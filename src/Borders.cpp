/*
 * Borders.cpp
 *
 *  Created on: Nov. 30, 2017
 *      Author: David Pastewka
 */

#include <Borders.h>

#include <algorithm>
#include <fstream>

#include <CGAL/squared_distance_3.h>
#include <CGAL/tags.h>

typedef CGAL::Tag_true  Supports_removal;


Borders::Borders(Polyhedron& mesh) {
  auto all_borders = findBorders(mesh);
  organizeBorders(all_borders);
}

Borders::~Borders() {
  // TODO Auto-generated destructor stub
}

vector<Halfedge_handle> Borders::findBorders(Polyhedron& mesh)
{
  // Call normalize_border to setup border DS -> CGAL precondition
  mesh.normalize_border();

  vector<Halfedge_handle> all_border_halfedge_handles;
  for (Halfedge_handle halfedge_handle = mesh.border_halfedges_begin();
      halfedge_handle != mesh.halfedges_end(); halfedge_handle++)
  {
    if (halfedge_handle->is_border())
    {
      all_border_halfedge_handles.push_back(halfedge_handle);
    }
  }
  return all_border_halfedge_handles;
}

// take all border handles and organize them based on the hole they surround
void Borders::organizeBorders(vector<Halfedge_handle> all_border_halfedge_handles)
{
  Borders::border current_border;
  while(!all_border_halfedge_handles.empty())
  {
    auto start = all_border_halfedge_handles[0];
    current_border.edges.push_back(start);
    auto current = start->next();
    while (current != start)
    {
      current_border.edges.push_back(current);
      current = current->next();
    }
    for (auto& handle : current_border.edges)
    {
      auto toerase = find(all_border_halfedge_handles.begin(),
          all_border_halfedge_handles.end(), handle);
      all_border_halfedge_handles.erase(toerase);
    }
    borders_.push_back(current_border);
    current_border.edges.clear();
  }
}

// Builds a border given an initial border halfedge
Borders::border Borders::buildBorder(Halfedge_handle halfedge)
{
  Borders::border border;

  if (!halfedge->is_border())
  {
    std::cout << "Initial halfedge is not a border halfedge\n";
    return border;
  }

  auto start = halfedge;
  border.edges.push_back(start);

  halfedge = halfedge->next();
  while (halfedge != start)
  {
    border.edges.push_back(halfedge);
    halfedge = halfedge->next();
  }
}

void Borders::sortBorders()
{
  std::sort(borders_.begin(), borders_.end(), [](Borders::border& first, Borders::border& second)
  {
      return first.edges.size() < second.edges.size();
  });
}

void Borders::calcBorderCentroids()
{
  for (auto& border : borders_)
  {
    border.centroid = calcBorderCentroid(border);
  }
}

Point_3 Borders::calcBorderCentroid(Borders::border border)
{
  float sumX(0), sumY(0), sumZ(0);
  for (auto& halfedge_handle : border.edges)
  {
    auto point = halfedge_handle->vertex()->point();
    sumX += point.x();
    sumY += point.y();
    sumZ += point.z();
  }
  int numOfPoints = border.edges.size();
  Point_3 centroid(sumX/(float)numOfPoints, sumY/(float)numOfPoints, sumZ/(float)numOfPoints);
  return centroid;
}

Point_3 Borders::centroidClosestTo(Point_3 point)
{
  float min_squared_distance(FLT_MAX);
  Point_3 closest_centroid;

  for (auto border : borders_)
  {
    auto centroid = border.centroid;
    auto squared_distance = CGAL::squared_distance (point, centroid);

    if (squared_distance < min_squared_distance)
    {
      min_squared_distance = squared_distance;
      closest_centroid = centroid;
    }
  }
  return closest_centroid;
}

void Borders::smoothBorders(int iterations)
{
  for (int i = 0; i < iterations; i++)
  {
    for (auto& border : borders_)
    {
      smoothBorder(border);
    }
  }
}

void Borders::smoothBorder(Borders::border border)
{
  int size = border.edges.size();
  vector<Point_3> new_pts;

  // Average first point on border
  new_pts.push_back(averagePoints(border.edges[0]->vertex()->point(),
      border.edges[size-1]->vertex()->point(), border.edges[1]->vertex()->point()));

  // Average middle points on border
  for (int i = 1; i < size-1; i++)
  {
    new_pts.push_back(averagePoints(border.edges[i]->vertex()->point(),
        border.edges[i-1]->vertex()->point(), border.edges[i+1]->vertex()->point()));
  }

  // Average last point on border
  new_pts.push_back(averagePoints(border.edges[size-1]->vertex()->point(),
      border.edges[size-2]->vertex()->point(), border.edges[0]->vertex()->point()));

  for (int i = 0; i < size; i++)
  {
    border.edges[i]->vertex()->point() = new_pts[i];
  }
}

// Iterates around border and deletes faces that fail deleteCondition
// Continues until all faces on border pass deleteCondition
vector<int> Borders::deleteFacesBelowY(Borders::border& border, Polyhedron& mesh, float y)
{
  bool done;
  vector<Halfedge_handle> to_delete;
  vector<int> to_delete_indices;
  do
  {
    Halfedge_handle passing_halfedge;
    done = true;
    for (auto& halfedge : border.edges) // iterate over all borders and mark for deletion
    {
      if (belowY(halfedge, y))
      {
        done = false;
        // TODO: Need to save index of deleted face
        if (!halfedge->opposite()->is_border()) // Make sure face hasn't already been deleted
        {
          to_delete.push_back(halfedge->opposite());
          to_delete_indices.push_back(halfedge->opposite()->facet()->id());

        }
      } else
      {
        passing_halfedge = halfedge; // rebuilding border after deletion for next iteration requires a border halfedge
      }
    }
    for (auto halfedge : to_delete)
    {
      if (halfedge->is_border()) continue;
      mesh.erase_facet(halfedge);
    }
    // std::cout << "num of faces deleted: " << to_delete.size() << endl;
    border = buildBorder(passing_halfedge);
    to_delete.clear();

    // // Write off file for debugging
    // std::ofstream os("dump.off");
    // if (!os) std::cout << "Couldn't open file for writing: " << endl;
    // os << mesh;
    // os.close();
    } while (!done);
    return to_delete_indices;
}

bool Borders::belowY(Halfedge_handle& halfedge, float y_max)
{
  if (halfedge->vertex()->point().y() < y_max)
  {
    return true;
  } else 
  {
    return false;
  }
}

Point_3 Borders::averagePoints(Point_3 middle, Point_3 before, Point_3 after)
{
  float new_x, new_y, new_z;
  new_x = middle.x()*0.5 + before.x()*0.25 + after.x()*0.25;
  new_y = middle.y()*0.5 + before.y()*0.25 + after.y()*0.25;
  new_z = middle.z()*0.5 + before.z()*0.25 + after.z()*0.25;

  return Point_3(new_x, new_y, new_z);
}


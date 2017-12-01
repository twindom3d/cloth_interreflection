/*
 * Borders.cpp
 *
 *  Created on: Nov. 30, 2017
 *      Author: David Pastewka
 */

#include <Borders.h>

#include <algorithm>

#include <CGAL/squared_distance_3.h>


Borders::Borders(Polyhedron& mesh) {
  findBorders(mesh);
  organizeBorders();
}

Borders::~Borders() {
  // TODO Auto-generated destructor stub
}

void Borders::findBorders(Polyhedron& mesh)
{
  // Call normalize_border to setup border DS -> CGAL precondition
  mesh.normalize_border();

  for (Halfedge_handle halfedge_handle = mesh.border_edges_begin();
      halfedge_handle != mesh.edges_end(); halfedge_handle++)
  {
    all_border_halfedge_handles_.push_back(halfedge_handle->opposite());
  }
}

// take all border handles and organize them based on the hole they surround
void Borders::organizeBorders()
{
  Borders::border current_border;
  while(!all_border_halfedge_handles_.empty())
  {
    auto start = all_border_halfedge_handles_[0];
    current_border.edges.push_back(start);
    auto current = start->next();
    while (current != start)
    {
      current_border.edges.push_back(current);
      current = current->next();
    }
    for (auto& handle : current_border.edges)
    {
      auto toerase = find(all_border_halfedge_handles_.begin(),
          all_border_halfedge_handles_.end(), handle);
      all_border_halfedge_handles_.erase(toerase);
    }
    borders_.push_back(current_border);
    current_border.edges.clear();
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

Point_3 Borders::averagePoints(Point_3 middle, Point_3 before, Point_3 after)
{
  float new_x, new_y, new_z;
  new_x = middle.x()*0.5 + before.x()*0.25 + after.x()*0.25;
  new_y = middle.y()*0.5 + before.y()*0.25 + after.y()*0.25;
  new_z = middle.z()*0.5 + before.z()*0.25 + after.z()*0.25;

  return Point_3(new_x, new_y, new_z);
}


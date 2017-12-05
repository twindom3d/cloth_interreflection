/* CleanInterreflectionsAppMain.cpp
 *
 *  Created on: Nov. 28, 2017
 *      Author: David Pastewka
 */

#include <iostream>
#include <fstream>
#include <algorithm>

#include <CGAL/IO/Polyhedron_iostream.h>

#include "Trimesh2/TriMesh.h"

#include <Borders.h>

float YSlope(Halfedge_handle halfedge)
{
  auto next_halfedge = halfedge;
  for (int i = 0; i < 3; i++)
  {
    next_halfedge = next_halfedge->next();
  }
  return (halfedge->vertex()->point().y() - next_halfedge->vertex()->point().y())/0.001;
}

struct isequal {
  isequal(int x) : x(x) {}
  bool operator()(Face f) const {
    return f.label == x;
  }
private:
  int x;
};



int main(int argc, char* argv[])
{
  char const* filenameInOff(argv[1]);
  char const* filenameInObj(argv[2]);
  char const* filenameOutObj(argv[3]);



  // Load off file
  std::ifstream stream(filenameInOff);

  if(!stream)
  {
        std::cout << "Cannot open file: " << filenameInOff <<"!";
  }

  Polyhedron mesh;
  stream >> mesh;

  // Initialize face ids
  int i(0);
  for(Polyhedron::Facet_iterator it = mesh.facets_begin(); it != mesh.facets_end(); ++it)
  {
    it->id() = i++;
  }

  // Find, organize and calculate border centroids
  Borders borders(mesh);
  borders.calcBorderCentroids();
  borders.sortBorders();
  reverse(borders.borders_.begin(), borders.borders_.end());
  borders.smoothBorders(10); // Smooth out jumps

  // Find arm borders
  // Assumed border furthest left = right arm and border furthest right = left arm
  Borders::border left_arm_border, right_arm_border;
  right_arm_border.centroid = Point_3(0,0,0);
  left_arm_border.centroid = Point_3(0,0,0);

  // Only consider x largest borders
  assert(borders.borders_.size() >= 4);
  for (int i = 0; i < 4; i++)
  {
    auto border = borders.borders_[i];

    if (border.centroid.x() > left_arm_border.centroid.x())
    {
      left_arm_border = border;
    }

    if (border.centroid.x() < right_arm_border.centroid.x())
    {
      right_arm_border = border;
    }
  }


  // Get min Y value for each arm hole by ...
  // Start with halfedge handle with point with greatest z-value (left arm)
  // Start with halfedge handle with point with smallest z-value (right arm)
  // Because CGAL lets you circle borders counter clockwise and we want to travel towards the body
  auto start_left_border = left_arm_border.edges[0];
  auto start_right_border = right_arm_border.edges[0];

  for (auto border_half_edge : left_arm_border.edges)
  {
    if (border_half_edge->vertex()->point().z() > start_left_border->vertex()->point().z())
    {
      start_left_border = border_half_edge;
    }
  }

  for (auto border_half_edge : right_arm_border.edges)
  {
    if (border_half_edge->vertex()->point().z() < start_right_border->vertex()->point().z())
    {
      start_right_border = border_half_edge;
    }
  }

  // From starting point, travel towards body, comparing y-slopes along the way
  // Left arm border
  // Only travel x% of the way around

  vector<float> left_arm_slopes_differences;
  auto current_halfedge = start_left_border->next();
  for (int i = 0; i < left_arm_border.edges.size() * 0.3; i++)
  {
    auto current_slope = YSlope(current_halfedge);
    current_halfedge = current_halfedge->next();
    auto three_forward = current_halfedge->next()->next();
    auto next_slope = YSlope(three_forward);
    float slope_diff = next_slope - current_slope;
    left_arm_slopes_differences.push_back(slope_diff);
    // std::cout << "slope_diff: " << slope_diff << " at pt: " << current_halfedge->vertex()->point() << endl;
  }

  // Right arm border
  vector<float> right_arm_slopes_differences;
  current_halfedge = start_right_border->next();
  for (int i = 0; i < right_arm_border.edges.size() * 0.3; i++)
  {
    auto current_slope = YSlope(current_halfedge);
    current_halfedge = current_halfedge->next();

    auto three_forward = current_halfedge->next()->next();
    auto next_slope = YSlope(three_forward);
    float slope_diff = next_slope - current_slope;
    right_arm_slopes_differences.push_back(slope_diff);
    // std::cout << "slope_diff: " << slope_diff << " at pt: " << current_halfedge->vertex()->point() << endl;
  }

  // Chose y-value at maximum slope diff
  auto max_slope_left = max_element(left_arm_slopes_differences.begin(), left_arm_slopes_differences.end());
  auto vertex_from_start_left = distance(left_arm_slopes_differences.begin(), max_slope_left);
  auto max_slope_right = max_element(right_arm_slopes_differences.begin(), right_arm_slopes_differences.end());
  auto vertex_from_start_right = distance(right_arm_slopes_differences.begin(), max_slope_right);

  Halfedge_handle cutoff_halfedge_left = start_left_border;
  for (int i = 0; i < vertex_from_start_left; i++)
  {
    cutoff_halfedge_left = cutoff_halfedge_left->next();
  }

  Halfedge_handle cutoff_halfedge_right = start_right_border;
  for (int i = 0; i < vertex_from_start_right; i++)
  {
    cutoff_halfedge_right = cutoff_halfedge_right->next();
  }

  auto cutoff_y_left = cutoff_halfedge_left->vertex()->point().y();
  auto cutoff_y_right = cutoff_halfedge_right->vertex()->point().y();


  std::cout << "cutoff left: " << cutoff_y_left << endl;
  std::cout << "cutoff right: " << cutoff_y_right << endl;
  // std::cout << "cd .start left: " << start_left_border->vertex()->point() << endl;
  // std::cout << "start right: " << start_right_border->vertex()->point() << endl;

  // Delete faces in arm hole below min Y value
  auto to_delete_left = borders.deleteFacesBelowY(left_arm_border, mesh, cutoff_y_left);
  auto to_delete_right = borders.deleteFacesBelowY(right_arm_border, mesh, cutoff_y_right);
  vector<int> to_delete_all;

  // Combine indices to delete into single vector
  to_delete_all.reserve( to_delete_left.size() + to_delete_right.size() ); // preallocate memory
  to_delete_all.insert( to_delete_all.end(), to_delete_left.begin(), to_delete_left.end() );
  to_delete_all.insert( to_delete_all.end(), to_delete_right.begin(), to_delete_right.end() );

  // Load obj into TriMesh, delete faces and save out
  TriMesh* trimesh;
  trimesh = trimesh->read(filenameInObj);
  trimesh->need_faces();
  trimesh->need_face_indices();

  for (auto i : to_delete_all)
  {
    trimesh->faces[i].label = 1;
  }
  vector<Face>::iterator it = std::remove_if(trimesh->faces.begin(), trimesh->faces.end(), isequal(1));
  trimesh->faces.erase(it, trimesh->faces.end());

  trimesh->write(filenameOutObj);
}


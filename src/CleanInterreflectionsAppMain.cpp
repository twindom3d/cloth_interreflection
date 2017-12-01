/* CleanInterreflectionsAppMain.cpp
 *
 *  Created on: Nov. 28, 2017
 *      Author: David Pastewka
 */

#include <iostream>
#include <fstream>
#include <algorithm>

#include <CGAL/IO/Polyhedron_iostream.h>

#include <Borders.h>

float YSlope(Halfedge_handle halfedge)
{
  auto next_halfedge = halfedge;
  for (int i = 0; i < 5; i++)
  {
    next_halfedge = next_halfedge->next();
  }
  return (halfedge->vertex()->point().y() - next_halfedge->vertex()->point().y())/0.001;
}

int main(int argc, char* argv[])
{
  char const* filenameIn(argv[1]);
  char const* filenameOut(argv[2]);



  // Load off file
  std::ifstream stream(filenameIn);

  if(!stream)
  {
        std::cout << "Cannot open file: " << filenameIn <<"!";
  }

  Polyhedron mesh;
  stream >> mesh;

  // Find, organize and calculate border centroids
  Borders borders(mesh);
  borders.calcBorderCentroids();
  borders.sortBorders();
  reverse(borders.borders_.begin(), borders.borders_.end());

  for (auto& border : borders.borders_)
  {
    std::cout << border.edges.size() << " ";
  }

  std::cout << endl;

  // borders.smoothBorders(3); // Smooth out jumps

  // Find arm borders
  // Assumed border furthest left = right arm and border furthest right = left arm
  // Only consider x largest borders
  Borders::border left_arm_border, right_arm_border;
  right_arm_border.centroid = Point_3(0,0,0);
  left_arm_border.centroid = Point_3(0,0,0);

  for (int i = 0; i < 6; i++)
  {
    auto border = borders.borders_[i];
    std::cout << "border size: " << border.edges.size() << endl;
    std::cout << "centroid: " << border.centroid << endl;
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
  // When slopes changes past threshold, choose this as minimum y

  // Left arm border
  // Only travel x% of the way around
  auto current_halfedge = start_left_border->next();
  for (int i = 0; i < left_arm_border.edges.size() * 0.3; i++)
  {
    auto current_slope = YSlope(current_halfedge);
    current_halfedge = current_halfedge->next();
    auto next_slope = YSlope(current_halfedge);
    float slope_diff = next_slope - current_slope;
    // std::cout << "slope_diff: " << slope_diff << " at pt: " << current_halfedge->vertex()->point() << endl;
  }

  std::cout << endl << endl << endl;

  // Right arm border
  current_halfedge = start_right_border->next();
  for (int i = 0; i < right_arm_border.edges.size() * 0.3; i++)
  {
    auto current_slope = YSlope(current_halfedge);
    current_halfedge = current_halfedge->next();
    auto next_slope = YSlope(current_halfedge);
    float slope_diff = next_slope - current_slope;
    // std::cout << "slope_diff: " << slope_diff << " at pt: " << current_halfedge->vertex()->point() << endl;
  }

  std::cout << "start left: " << start_left_border->vertex()->point() << endl;
  std::cout << "start right: " << start_right_border->vertex()->point() << endl;

  // Delete faces in arm hole below min Y value

  // Save off file
}

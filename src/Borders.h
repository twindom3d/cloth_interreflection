/*
 * Borders.h
 *
 *  Created on: Nov. 30, 2017
 *      Author: David Pastewka
 */

#ifndef BORDERS_
#define BORDERS_

#include <CGAL/Simple_cartesian.h>
#include "CGAL/Polyhedron_items_3.h"
#include "CGAL/Polyhedron_3.h"
#include "CGAL/HalfedgeDS_list.h"

typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Polyhedron_3<Kernel,CGAL::Polyhedron_items_3,CGAL::HalfedgeDS_list> Polyhedron;
typedef Polyhedron::Halfedge_handle Halfedge_handle;
typedef CGAL::Point_3<Kernel> Point_3;

using namespace std;

class Borders {
public:

  struct border{
    vector<Halfedge_handle> edges;
    Point_3 centroid;
  };

  vector<Halfedge_handle> all_border_halfedge_handles_;
  vector<border> borders_;

  Borders(Polyhedron&);
  virtual ~Borders();

  void findBorders(Polyhedron&);
  void organizeBorders();
  void sortBorders();
  void calcBorderCentroids();
  Point_3 calcBorderCentroid(border);
  Point_3 centroidClosestTo(Point_3);
  void smoothBorders(int iterations);
  void smoothBorder(border);
  Point_3 averagePoints(Point_3 middle, Point_3 before, Point_3 after);
};

#endif /* BORDERS_ */
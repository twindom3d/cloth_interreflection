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
#include "CGAL/Polyhedron_items_with_id_3.h"
#include "CGAL/HalfedgeDS_list.h"

typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Polyhedron_3<Kernel,CGAL::Polyhedron_items_with_id_3,CGAL::HalfedgeDS_list> Polyhedron;
typedef Polyhedron::Halfedge_handle Halfedge_handle;
typedef CGAL::Point_3<Kernel> Point_3;

using namespace std;

class Borders {
public:

  struct border{
    vector<Halfedge_handle> edges;
    Point_3 centroid;
  };

  vector<border> borders_;

  Borders(Polyhedron&);
  virtual ~Borders();

  vector<Halfedge_handle> findBorders(Polyhedron&);
  void organizeBorders(vector<Halfedge_handle>);
  border buildBorder(Halfedge_handle);
  void sortBorders();
  void calcBorderCentroids();
  Point_3 calcBorderCentroid(border);
  Point_3 centroidClosestTo(Point_3);
  void smoothBorders(int iterations);
  void smoothBorder(border);
  vector<int> deleteFacesBelowY(border&, Polyhedron&, float);
  bool belowY(Halfedge_handle&, float);

  Point_3 averagePoints(Point_3 middle, Point_3 before, Point_3 after);
};

#endif /* BORDERS_ */
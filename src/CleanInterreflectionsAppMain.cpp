/* CleanInterreflectionsAppMain.cpp
 *
 *  Created on: Nov. 28, 2017
 *      Author: David Pastewka
 */

#include <iostream>
#include <iterator>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Utils/getopt.h>


using namespace OpenMesh;

typedef TriMesh_ArrayKernelT<>  MyMesh;


  // Load off file

  // Find, organize and calculate border centroids

  // Find arm holes

  // Get min Y value for each arm hole

  // Delete faces in arm hole below min Y value

  // Save off file






int main(int argc, char* argv[])
{
  char const* filenameIn(argv[1]);
  char const* filenameOut(argv[2]);


  MyMesh mesh;
  IO::Options ropt, wopt;

  // mesh.request_halfedge_texcoords2D();
  mesh.request_vertex_texcoords2D();
  // mesh.request_vertex_normals();

  IO::Options readOptions, writeOptions;
  readOptions += IO::Options::VertexTexCoord;
  // readOptions += IO::Options::VertexNormal;

  writeOptions += IO::Options::VertexTexCoord;
  // writeOptions += IO::Options::VertexNormal;

  // std::cout << "write textcoords? " << writeOptions.check(IO::Options::VertexTexCoord) << "\n";

  if (!OpenMesh::IO::read_mesh(mesh, filenameIn, readOptions)) 
  {
    std::cerr << "read error\n";
    exit(1);
  }

  // std::cout << "text coord: " << mesh.texcoord2D(mesh.halfedge_handle(0))[0] << "\n";

  if (!OpenMesh::IO::write_mesh(mesh, filenameOut, writeOptions)) 
  {
    std::cerr << "write error\n";
    exit(1);
  }
}

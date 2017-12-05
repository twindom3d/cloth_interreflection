project(clothe_interreflections)
cmake_minimum_required(VERSION 2.8)

set(GCC_COMPILE_FLAGS "-std=c++11 -O3 -frounding-math")
add_definitions(${GCC_COMPILE_FLAGS})

link_directories(/home/will/Downloads/cgal-releases-CGAL-4.8/build/lib/)
include_directories(${PROJECT_SOURCE_DIR})

add_executable(cleaninterreflections CleanInterreflectionsAppMain.cpp Borders.cpp Trimesh2/diffuse.cc Trimesh2/edgeflip.cc Trimesh2/faceflip.cc Trimesh2/filter.cc Trimesh2/ICP.cc Trimesh2/KDtree.cc Trimesh2/lmsmooth.cc Trimesh2/remove.cc Trimesh2/reorder_verts.cc Trimesh2/subdiv.cc Trimesh2/TriMesh_bounding.cc Trimesh2/TriMesh_connectivity.cc Trimesh2/TriMesh_curvature.cc Trimesh2/TriMesh_grid.cc Trimesh2/TriMesh_io.cc Trimesh2/TriMesh_normals.cc Trimesh2/TriMesh_pointareas.cc Trimesh2/TriMesh_stats.cc Trimesh2/TriMesh_tstrips.cc)

TARGET_LINK_LIBRARIES(cleaninterreflections CGAL)

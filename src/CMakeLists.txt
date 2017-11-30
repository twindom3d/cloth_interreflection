project(clothe_interreflections)
cmake_minimum_required(VERSION 2.8)

link_directories(${PROJECT_SOURCE_DIR}/../lib/OpenMesh-6.3/build/Build/lib/)
include_directories(${PROJECT_SOURCE_DIR}/../lib/OpenMesh-6.3/src/)

add_executable(cleaninterreflections CleanInterreflectionsAppMain.cpp)

TARGET_LINK_LIBRARIES(cleaninterreflections OpenMeshCore OpenMeshTools)

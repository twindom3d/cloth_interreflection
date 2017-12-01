project(clothe_interreflections)
cmake_minimum_required(VERSION 2.8)

set(GCC_COMPILE_FLAGS "-std=c++11 -O3 -frounding-math")
add_definitions(${GCC_COMPILE_FLAGS})

link_directories(/home/will/Downloads/cgal-releases-CGAL-4.8/build/lib/)
include_directories(${PROJECT_SOURCE_DIR})

add_executable(cleaninterreflections CleanInterreflectionsAppMain.cpp Borders.cpp)

TARGET_LINK_LIBRARIES(cleaninterreflections CGAL)

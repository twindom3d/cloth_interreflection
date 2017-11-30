project(clothe_interreflections)
cmake_minimum_required(VERSION 2.8)

link_directories()
include_directories()

add_executable(cleaninterreflections CleanInterreflectionsAppMain.cpp)

TARGET_LINK_LIBRARIES(cleaninterreflections )

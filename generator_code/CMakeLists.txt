cmake_minimum_required(VERSION 3.1) 
project(VideoWriter VERSION 1.0)            

# set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3 -Wno-write-strings")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,$ORIGIN")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -Wno-write-strings")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,$ORIGIN")
#opencv
find_package(OpenCV REQUIRED)

#main executable
add_executable(vidWriter VideoGenerator.cpp)
target_link_libraries(vidWriter ${OpenCV_LIBS})

add_executable(orderedWriter videoGeneratorOpenCV.cpp)
target_link_libraries(orderedWriter ${OpenCV_LIBS})

add_executable(syncro syncro.cpp)
target_include_directories(syncro PUBLIC ${GSTREAMER_INCLUDE_DIRS})
target_link_libraries(syncro ${GSTREAMER_LIBRARIES} ${OpenCV_LIBS})
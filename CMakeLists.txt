cmake_minimum_required(VERSION 3.1) 
project(FlirTau2Acq)

find_package(PkgConfig REQUIRED)
find_package(OpenCV REQUIRED) #FOR OPENCV - GSTREAMER

# NOT NEEDED - finds glib.h
# pkg_search_module(GLIB REQUIRED glib-2.0)
# finds gstreamer
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
# NOT NEEDED - 
# pkg_check_modules(GST_APP REQUIRED gstreamer-app-1.0)
# pkg_check_modules(GST_VIDEO REQUIRED gstreamer-video-1.0)

set(CMAKE_C_COMPILER "/usr/bin/g++")

add_executable(${PROJECT_NAME} FlirTau2.cpp)
# includes gst
target_include_directories(${PROJECT_NAME} PUBLIC ${GSTREAMER_INCLUDE_DIRS})
# links gstreamer libraries to access all functionalities
target_link_libraries(${PROJECT_NAME} ${GSTREAMER_LIBRARIES})

add_executable(gstcv FlirGstOpenCV.cpp)
target_include_directories(gstcv PUBLIC ${GSTREAMER_INCLUDE_DIRS})
target_link_libraries(gstcv ${GSTREAMER_LIBRARIES} ${OpenCV_LIBS})

add_executable(onlygstcv GstOpenCV.cpp)
target_include_directories(onlygstcv PUBLIC ${GSTREAMER_INCLUDE_DIRS})
target_link_libraries(onlygstcv ${GSTREAMER_LIBRARIES} ${OpenCV_LIBS})

add_executable(syncro syncro.cpp)
target_include_directories(syncro PUBLIC ${GSTREAMER_INCLUDE_DIRS})
target_link_libraries(syncro ${GSTREAMER_LIBRARIES} ${OpenCV_LIBS})
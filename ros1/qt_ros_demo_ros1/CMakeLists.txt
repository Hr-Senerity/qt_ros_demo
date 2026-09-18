cmake_minimum_required(VERSION 3.10)
project(qt_ros_demo_ros1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(catkin REQUIRED COMPONENTS roscpp geometry_msgs nav_msgs)
find_package(Qt5 REQUIRED COMPONENTS Widgets)

catkin_package()

# 共享同一份 common 源码：把 common/ 作为子目录编成静态库
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../common
                 ${CMAKE_BINARY_DIR}/common)

include_directories(${catkin_INCLUDE_DIRS})

add_executable(qt_gui_node
    src/main.cpp
    src/ros1_model.h
    src/ros1_model.cpp)
target_link_libraries(qt_gui_node
    ${catkin_LIBRARIES}
    Qt5::Widgets
    qt_ros_demo_common)

install(TARGETS qt_gui_node
        RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})
install(DIRECTORY launch
        DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})

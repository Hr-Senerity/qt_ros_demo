cmake_minimum_required(VERSION 3.16)
project(qt_ros_demo_ros2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# 共享同一份 common 源码：把 common/ 作为子目录编成静态库
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../common
                 ${CMAKE_BINARY_DIR}/common)

add_executable(qt_gui_node
    src/main.cpp
    src/ros2_model.h
    src/ros2_model.cpp)
ament_target_dependencies(qt_gui_node rclcpp geometry_msgs nav_msgs)
target_link_libraries(qt_gui_node
    Qt5::Widgets
    qt_ros_demo_common)

install(TARGETS qt_gui_node
        DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch
        DESTINATION share/${PROJECT_NAME})

ament_package()

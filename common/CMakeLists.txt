cmake_minimum_required(VERSION 3.10)
project(qt_ros_demo_common LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Qt5（ROS1/ROS2 各发行版自带）与 Qt6 均可
find_package(Qt5 COMPONENTS Widgets QUIET)
if(Qt5_FOUND)
    set(QT_PKG Qt5)
else()
    set(QT_PKG Qt6)
endif()
find_package(${QT_PKG} REQUIRED COMPONENTS Widgets)

add_library(qt_ros_demo_common STATIC
    include/qt_ros_demo/robot_model.h
    include/qt_ros_demo/widget.h
    src/widget.cpp)
target_include_directories(qt_ros_demo_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(qt_ros_demo_common PUBLIC ${QT_PKG}::Widgets)

if(MSVC)
    # 源码含中文，MSVC 需要显式声明 UTF-8
    target_compile_options(qt_ros_demo_common PRIVATE /utf-8)
endif()

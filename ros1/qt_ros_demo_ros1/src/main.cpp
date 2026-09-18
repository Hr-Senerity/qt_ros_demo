#include <QApplication>

#include <ros/exceptions.h>
#include <ros/ros.h>

#include <qt_ros_demo/widget.h>

#include "ros1_model.h"

int main(int argc, char* argv[]) {
    // 先起 Qt：QApplication 会把 -style 等 Qt 参数从 argv 里清掉，ROS 再解析剩余参数。
    QApplication app(argc, argv);

    try {
        ros::init(argc, argv, "qt_gui_node");
    } catch (const ros::RosInitException& e) {
        qFatal("ROS 初始化失败：%s", e.what());
    }

    Ros1Model model;
    Widget w(&model);
    w.show();

    if (!model.start()) {
        return 1;
    }
    const int ret = app.exec();
    model.stop();  // 先停 ROS 线程，再让 Qt 对象析构
    return ret;
}

#pragma once

#include <QObject>
#include <QString>

// UI 与 ROS 之间唯一的边界：widget.h 只认识这个接口，不知道背后是 ROS1 还是 ROS2。
//
// 线程纪律（ros1/、ros2/ 两个实现都必须遵守）：
//   1. start() 把 ROS 的 spin 循环放到子线程，Qt 主线程只跑界面事件循环；
//   2. ROS 回调里只 emit 信号（跨线程时 Qt 自动走队列连接），不直接碰 UI；
//   3. publishVelocity() 在 UI 线程被调用，实现内部保证与回调线程并发安全；
//   4. 退出时先 stop()（停 spin、join 子线程），再析构 Qt 对象。
class RobotModel : public QObject {
    Q_OBJECT

public:
    explicit RobotModel(QObject* parent = nullptr) : QObject(parent) {}
    ~RobotModel() override = default;

    // 启动后台通信线程。调用前 main.cpp 必须已完成 ros::init / rclcpp::init。
    virtual bool start() = 0;

    // 停止后台线程并 join，只在程序退出时调用一次。
    virtual void stop() = 0;

    // 发布速度指令（线速度 m/s，角速度 rad/s）。
    virtual void publishVelocity(double linear, double angular) = 0;

signals:
    // 收到的里程计位姿：ROS 线程 -> UI 线程。
    void poseUpdated(double x, double y, double theta);
    // 连接状态提示，显示在窗口标题上。
    void statusChanged(const QString& message);
};

#pragma once

#include <atomic>
#include <thread>

#include <geometry_msgs/msg/twist.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <rclcpp/rclcpp.hpp>

#include <qt_ros_demo/robot_model.h>

// RobotModel 的 ROS2（rclcpp）实现。rclcpp::init 由 main.cpp 在 QApplication 之后完成。
class Ros2Model : public RobotModel {
    Q_OBJECT

public:
    ~Ros2Model() override;

    bool start() override;
    void stop() override;
    void publishVelocity(double linear, double angular) override;

private:
    rclcpp::Node::SharedPtr node_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmdVelPub_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odomSub_;
    std::thread spinThread_;
    std::atomic_bool running_{false};
};

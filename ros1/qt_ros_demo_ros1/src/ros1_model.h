#pragma once

#include <atomic>
#include <thread>

#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <ros/ros.h>

#include <qt_ros_demo/robot_model.h>

// RobotModel 的 ROS1（roscpp）实现。ros::init 由 main.cpp 在 QApplication 之后完成。
class Ros1Model : public RobotModel {
    Q_OBJECT

public:
    ~Ros1Model() override;

    bool start() override;
    void stop() override;
    void publishVelocity(double linear, double angular) override;

private:
    void odomCallback(const nav_msgs::Odometry::ConstPtr& msg);

    ros::Publisher cmdVelPub_;
    ros::Subscriber odomSub_;
    std::thread spinThread_;
    std::atomic_bool running_{false};
};

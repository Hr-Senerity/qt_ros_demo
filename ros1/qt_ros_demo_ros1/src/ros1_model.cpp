#include "ros1_model.h"

#include <cmath>

Ros1Model::~Ros1Model() { stop(); }

bool Ros1Model::start() {
    if (running_.exchange(true)) {
        return true;
    }

    ros::NodeHandle nh;
    cmdVelPub_ = nh.advertise<geometry_msgs::Twist>("cmd_vel", 10);
    odomSub_ = nh.subscribe("odom", 10, &Ros1Model::odomCallback, this);

    spinThread_ = std::thread([this] { ros::spin(); });

    emit statusChanged(tr("ROS1：已订阅 /odom，等待数据…"));
    return true;
}

void Ros1Model::stop() {
    if (!running_.exchange(false)) {
        return;
    }
    ros::shutdown();  // 让子线程里的 ros::spin() 返回
    if (spinThread_.joinable()) {
        spinThread_.join();
    }
}

void Ros1Model::publishVelocity(double linear, double angular) {
    if (!running_ || !cmdVelPub_) {
        return;
    }
    geometry_msgs::Twist msg;
    msg.linear.x = linear;
    msg.angular.z = angular;
    cmdVelPub_.publish(msg);  // roscpp 的 publish 线程安全，可从 UI 线程直接调用
}

void Ros1Model::odomCallback(const nav_msgs::Odometry::ConstPtr& msg) {
    const auto& p = msg->pose.pose.position;
    const auto& q = msg->pose.pose.orientation;
    // 四元数 -> 偏航角，不引入 tf 依赖
    const double yaw = std::atan2(2.0 * (q.w * q.z + q.x * q.y),
                                  1.0 - 2.0 * (q.y * q.y + q.z * q.z));
    emit poseUpdated(p.x, p.y, yaw);  // 在 ROS 线程 emit，Qt 自动队列化到 UI 线程
}

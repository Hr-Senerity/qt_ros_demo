#include "ros2_model.h"

#include <cmath>

Ros2Model::~Ros2Model() { stop(); }

bool Ros2Model::start() {
    if (running_.exchange(true)) {
        return true;
    }

    node_ = rclcpp::Node::make_shared("qt_gui_node");
    cmdVelPub_ = node_->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
    odomSub_ = node_->create_subscription<nav_msgs::msg::Odometry>(
        "odom", 10,
        [this](nav_msgs::msg::Odometry::ConstSharedPtr msg) {
            const auto& p = msg->pose.pose.position;
            const auto& q = msg->pose.pose.orientation;
            // 四元数 -> 偏航角，不引入 tf 依赖
            const double yaw = std::atan2(2.0 * (q.w * q.z + q.x * q.y),
                                          1.0 - 2.0 * (q.y * q.y + q.z * q.z));
            emit poseUpdated(p.x, p.y, yaw);  // 在 ROS 线程 emit，Qt 自动队列化到 UI 线程
        });

    spinThread_ = std::thread([this] { rclcpp::spin(node_); });

    emit statusChanged(tr("ROS2：已订阅 /odom，等待数据…"));
    return true;
}

void Ros2Model::stop() {
    if (!running_.exchange(false)) {
        return;
    }
    rclcpp::shutdown();  // 让子线程里的 rclcpp::spin() 返回
    if (spinThread_.joinable()) {
        spinThread_.join();
    }
    node_.reset();
}

void Ros2Model::publishVelocity(double linear, double angular) {
    if (!running_ || !node_) {
        return;
    }
    geometry_msgs::msg::Twist msg;
    msg.linear.x = linear;
    msg.angular.z = angular;
    cmdVelPub_->publish(msg);  // rclcpp 的 publish 线程安全，可从 UI 线程直接调用
}

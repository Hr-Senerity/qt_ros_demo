#pragma once

#include <QWidget>

#include "qt_ros_demo/robot_model.h"

class QDoubleSpinBox;
class QLineEdit;
class QPushButton;

// 主界面：订阅 /odom 显示位姿，输入线/角速度后手动发布 /cmd_vel。
// 纯 Qt 实现，不含任何 ROS 头文件。
class Widget : public QWidget {
    Q_OBJECT

public:
    explicit Widget(RobotModel* model, QWidget* parent = nullptr);

private:
    RobotModel* model_;

    QLineEdit* poseX_ = nullptr;
    QLineEdit* poseY_ = nullptr;
    QLineEdit* poseTheta_ = nullptr;
    QDoubleSpinBox* linearSpin_ = nullptr;
    QDoubleSpinBox* angularSpin_ = nullptr;
    QPushButton* publishButton_ = nullptr;
};

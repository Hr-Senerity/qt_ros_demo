#include "qt_ros_demo/widget.h"

#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

Widget::Widget(RobotModel* model, QWidget* parent) : QWidget(parent), model_(model) {
    setWindowTitle(tr("Qt + ROS 上位机"));
    resize(380, 260);

    auto* poseGroup = new QGroupBox(tr("位姿（订阅 /odom）"), this);
    auto* poseForm = new QFormLayout(poseGroup);
    poseX_ = new QLineEdit(poseGroup);
    poseY_ = new QLineEdit(poseGroup);
    poseTheta_ = new QLineEdit(poseGroup);
    for (QLineEdit* edit : {poseX_, poseY_, poseTheta_}) {
        edit->setReadOnly(true);
        edit->setAlignment(Qt::AlignRight);
        edit->setPlaceholderText(tr("等待数据…"));
    }
    poseForm->addRow(tr("X (m)"), poseX_);
    poseForm->addRow(tr("Y (m)"), poseY_);
    poseForm->addRow(tr("航向角 θ"), poseTheta_);

    auto* cmdGroup = new QGroupBox(tr("速度指令（发布 /cmd_vel）"), this);
    auto* cmdForm = new QFormLayout(cmdGroup);
    linearSpin_ = new QDoubleSpinBox(cmdGroup);
    linearSpin_->setRange(-1.0, 2.0);
    linearSpin_->setSingleStep(0.05);
    linearSpin_->setSuffix(tr(" m/s"));
    angularSpin_ = new QDoubleSpinBox(cmdGroup);
    angularSpin_->setRange(-3.14, 3.14);
    angularSpin_->setSingleStep(0.05);
    angularSpin_->setSuffix(tr(" rad/s"));
    publishButton_ = new QPushButton(tr("发布一次"), cmdGroup);
    cmdForm->addRow(tr("线速度 v"), linearSpin_);
    cmdForm->addRow(tr("角速度 ω"), angularSpin_);
    cmdForm->addRow(publishButton_);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(poseGroup);
    layout->addWidget(cmdGroup);
    layout->addStretch(1);

    // ROS 线程 emit、UI 线程接收：跨线程 connect 默认就是队列连接，无需加锁。
    connect(model_, &RobotModel::poseUpdated, this, [this](double x, double y, double theta) {
        poseX_->setText(QString::number(x, 'f', 3));
        poseY_->setText(QString::number(y, 'f', 3));
        poseTheta_->setText(QString::number(theta, 'f', 3) + tr(" rad"));
    });
    connect(model_, &RobotModel::statusChanged, this, &Widget::setWindowTitle);

    connect(publishButton_, &QPushButton::clicked, this, [this] {
        model_->publishVelocity(linearSpin_->value(), angularSpin_->value());
    });
}

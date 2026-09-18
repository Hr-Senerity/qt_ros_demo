# qt_ros_demo

基于 Qt 的机器人上位机 GUI 演示框架，目标是**同一套界面代码兼容 ROS1（Noetic）与 ROS2（Foxy/Humble）**。

> 由原仓库 `qt_netic_demo`（ROS-Noetic 单版本 demo）更名而来，当前为最小可编译骨架：一套界面，两个 ROS 薄壳。
> 环境搭建参考：[一站式搭建 QT 上位机 ROS 共存（noetic+foxy）开发平台](https://blog.csdn.net/qq_46274948/article/details/132022388)

## 核心思路

- **界面与 ROS 解耦**：`common/` 为纯 Qt 库，不含任何 ROS 头文件；ROS1/ROS2 通过各自的适配层实现同一抽象接口，UI 只面向接口编程。
- **两套薄壳工程**：ROS1 用 catkin，ROS2 用 colcon，两者本质上都是标准 CMake 工程，可分别导入 Qt Creator。
- **线程模型**：ROS 通信运行在独立线程，通过 Qt 信号槽（跨线程队列连接）把数据送到 UI 线程；UI 线程不直接接触任何 ROS 对象。

## 目录结构

```
qt_ros_demo/
├── common/                    # 纯 Qt 界面库（无 ROS 依赖，编成静态库）
│   ├── include/qt_ros_demo/
│   │   ├── robot_model.h      # RobotModel 抽象接口：UI 与 ROS 的唯一边界
│   │   └── widget.h           # 主界面 Widget
│   └── src/widget.cpp
├── ros1/
│   └── qt_ros_demo_ros1/      # ROS1 (Noetic) catkin 薄壳：Ros1Model + roscpp
└── ros2/
    └── qt_ros_demo_ros2/      # ROS2 (Foxy/Humble) colcon 薄壳：Ros2Model + rclcpp
```

两个薄壳包各自用 `add_subdirectory(../../common)` 把共享界面库编进可执行文件，所以**使用时要把整个仓库放进（或软链到）工作空间 `src/` 下**，不要只拷贝单个包；`ros1/` 内置 `COLCON_IGNORE`、`ros2/` 内置 `CATKIN_IGNORE`，放哪一侧都不会被对方误扫。

UI 与 ROS 的全部交互收敛在 `RobotModel` 接口上：ROS 的 spin 循环在子线程里跑，回调只 emit `poseUpdated` 信号（跨线程自动走队列连接），UI 线程点"发布"按钮经 `publishVelocity()` 发速度指令。话题约定：订阅 `/odom`（nav_msgs/Odometry），发布 `/cmd_vel`（geometry_msgs/Twist）。

## 快速开始

### ROS1（Noetic）

```bash
mkdir -p ~/catkin_ws/src
ln -s /path/to/qt_ros_demo ~/catkin_ws/src/qt_ros_demo   # 或把仓库直接 clone 到 src/ 下
cd ~/catkin_ws && catkin_make
source devel/setup.bash
rosrun qt_ros_demo_ros1 qt_gui_node
# 或：roslaunch qt_ros_demo_ros1 qt_gui_node.launch
```

命令行发假数据验证界面：`rostopic pub -r 10 /odom nav_msgs/Odometry '{pose: {pose: {position: {x: 1.0, y: 0.5}}}}'`

Qt Creator：从已 source noetic 的终端启动；如需 ROS 工程模板/集成，安装与 Creator 版本匹配的 [ros_qtc_plugin](https://github.com/ros-industrial/ros_qtc_plugin/releases)。

### ROS2（Foxy/Humble）

```bash
mkdir -p ~/ros2_ws/src
ln -s /path/to/qt_ros_demo ~/ros2_ws/src/qt_ros_demo
cd ~/ros2_ws && colcon build
source install/setup.bash
ros2 run qt_ros_demo_ros2 qt_gui_node
# 或：ros2 launch qt_ros_demo_ros2 qt_gui_node.launch.py
```

命令行发假数据验证界面：`ros2 topic pub -r 10 /odom nav_msgs/msg/Odometry '{pose: {pose: {position: {x: 1.0, y: 0.5}}}}'`

Qt Creator：**先 colcon build**，再打开 `ros2/qt_ros_demo_ros2/CMakeLists.txt`，在配置页导入 `build/qt_ros_demo_ros2` 目录，复用 colcon 的编译配置（不需要任何插件）。

## 开发流程要点

1. 命令行编译为准（`catkin_make` / `colcon build`），Qt Creator 只做编码、断点调试与运行；
2. 从 source 过对应 ROS 版本的终端启动 Qt Creator，保证环境变量继承；
3. ROS 回调线程不触碰任何 UI 对象，统一走信号槽跨线程通信；
4. 演示阶段只用 std_msgs / geometry_msgs / sensor_msgs，避免双版本自定义消息生成的麻烦；
5. 双 ROS 共存时一个终端只 source 一个版本；需要严格隔离时使用 Docker。

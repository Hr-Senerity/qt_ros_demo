from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    # 话题重映射示例：remappings=[('odom', '/robot1/odom')]
    return LaunchDescription([
        Node(
            package='qt_ros_demo_ros2',
            executable='qt_gui_node',
            output='screen',
        ),
    ])

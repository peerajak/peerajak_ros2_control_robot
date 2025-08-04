# This is to understand ros2_control

Using this package together with peerajak_ros2_control_robot_description

## How to run

### 1. Compile

```
cd ros2_ws; 
colcon build --packages-select peerajak_ros2_control_robot_bringup peerajak_ros2_control_robot_description
source install/setup.bash
```

### 2. Run

#### 2.1

Terminal 1

```
ros2 run robot_state_publisher robot_state_publisher --ros-args -p robot_description:="$(xacro /home/peerajak/ros2_ws/src/peerajak_ros2_control_robot_description/urdf/peerajak_ros2_control_robot.urdf.xacro)"
```

Terminal 2

```
ros2 run controller_manager ros2_control_node --ros-args -r /controller_manager/robot_description:=/robot_description --params-file /home/peerajak/ros2_ws/src/peerajak_ros2_control_robot_bringup/config/peerajak_ros2_control_robot_controllers.yaml
```

Terminal 3

```
ros2 run controller_manager spawner joint_state_broadcaster
ros2 run controller_manager spawner diff_drive_controller
```

Terminal 4: testing with teleop

```
ros2 run teleop_twist_keyboard teleop_twist_keyboard --ros-args -r /cmd_vel:=/diff_drive_controller/cmd_vel -p stamped:=true
```

#### 2.2 run with launch file

- method 1

```
ros2 launch peerajak_ros2_control_robot_bringup peerajak_ros2_control_robot.launch.xml
```

- method 2

```
ros2 launch peerajak_ros2_control_robot_bringup peerajak_ros2_control_robot.launch.py
```

#### Other helper commands

- 1.
```
ros2 control list_controllers
```

expected output

```
joint_state_broadcaster joint_state_broadcaster/JointStateBroadcaster  active
diff_drive_controller   diff_drive_controller/DiffDriveController      active
```

- 2.
```
ros2 control list_controller_types
```

expected output 

```
controller_manager/test_controller                                     controller_interface::ControllerInterface
controller_manager/test_controller_failed_init                         controller_interface::ControllerInterface
controller_manager/test_controller_with_interfaces                     controller_interface::ControllerInterface
diff_drive_controller/DiffDriveController                              controller_interface::ControllerInterface
effort_controllers/GripperActionController                             controller_interface::ControllerInterface
effort_controllers/JointGroupEffortController                          controller_interface::ControllerInterface
force_torque_sensor_broadcaster/ForceTorqueSensorBroadcaster           controller_interface::ControllerInterface
forward_command_controller/ForwardCommandController                    controller_interface::ControllerInterface
forward_command_controller/MultiInterfaceForwardCommandController      controller_interface::ControllerInterface
gpio_controllers/GpioCommandController                                 controller_interface::ControllerInterface
imu_sensor_broadcaster/IMUSensorBroadcaster                            controller_interface::ControllerInterface
joint_state_broadcaster/JointStateBroadcaster                          controller_interface::ControllerInterface
joint_trajectory_controller/JointTrajectoryController                  controller_interface::ControllerInterface
mecanum_drive_controller/MecanumDriveController                        controller_interface::ControllerInterface
pose_broadcaster/PoseBroadcaster                                       controller_interface::ControllerInterface
position_controllers/GripperActionController                           controller_interface::ControllerInterface
position_controllers/JointGroupPositionController                      controller_interface::ControllerInterface
range_sensor_broadcaster/RangeSensorBroadcaster                        controller_interface::ControllerInterface
ros2_control_demo_example_7/RobotController                            controller_interface::ControllerInterface
tricycle_controller/TricycleController                                 controller_interface::ControllerInterface
ur_controllers/ForceModeController                                     controller_interface::ControllerInterface
ur_controllers/FreedriveModeController                                 controller_interface::ControllerInterface
ur_controllers/GPIOController                                          controller_interface::ControllerInterface
ur_controllers/PassthroughTrajectoryController                         controller_interface::ControllerInterface
ur_controllers/ScaledJointTrajectoryController                         controller_interface::ControllerInterface
ur_controllers/SpeedScalingStateBroadcaster                            controller_interface::ControllerInterface
ur_controllers/URConfigurationController                               controller_interface::ControllerInterface
velocity_controllers/JointGroupVelocityController                      controller_interface::ControllerInterface
ackermann_steering_controller/AckermannSteeringController              controller_interface::ChainableControllerInterface
admittance_controller/AdmittanceController                             controller_interface::ChainableControllerInterface
bicycle_steering_controller/BicycleSteeringController                  controller_interface::ChainableControllerInterface
controller_manager/test_chainable_controller                           controller_interface::ChainableControllerInterface
passthrough_controller/PassthroughController                           controller_interface::ChainableControllerInterface
pid_controller/PidController                                           controller_interface::ChainableControllerInterface
tricycle_steering_controller/TricycleSteeringController                controller_interface::ChainableControllerInterface
```

- 3.

```
ros2 control list_hardware_interfaces
```

expected output 

```
command interfaces
	base_left_wheel_joint/velocity [available] [claimed]
	base_right_wheel_joint/velocity [available] [claimed]
state interfaces
	base_left_wheel_joint/position
	base_left_wheel_joint/velocity
	base_right_wheel_joint/position
	base_right_wheel_joint/velocity
```

- 4.


```
ros2 control list_hardware_components
```

expected output 

```
Hardware Component 1
	name: MobileBaseHardwareInterface
	type: system
	plugin name: mock_components/GenericSystem
	state: id=3 label=active
	command interfaces
		base_right_wheel_joint/velocity [available] [claimed]
		base_left_wheel_joint/velocity [available] [claimed]
```
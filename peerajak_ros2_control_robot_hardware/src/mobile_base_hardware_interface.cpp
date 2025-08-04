#include "peerajak_ros2_control_robot_hardware/mobile_base_hardware_interface.hpp"
#include <hardware_interface/types/hardware_interface_type_values.hpp>
#include <pluginlib/class_list_macros.hpp>


namespace mobile_base_hardware {

hardware_interface::CallbackReturn MobileBaseHardwareInterface::on_init
    (const hardware_interface::HardwareInfo & hardware_info)
{
    if (hardware_interface::SystemInterface::on_init(hardware_info) !=
        hardware_interface::CallbackReturn::SUCCESS)
    {
        return hardware_interface::CallbackReturn::ERROR;
    }

    //info_ = info;
    RCLCPP_INFO(rclcpp::get_logger("MobileBaseHardwareInterface"), info_.hardware_parameters["left_motor_id"].c_str());
    left_motor_id_ = std::stoi(info_.hardware_parameters["left_motor_id"]);
    right_motor_id_ = std::stoi(info_.hardware_parameters["right_motor_id"]);
    port_ = info_.hardware_parameters["dynamixel_port"];
    driver_ = std::make_shared<XL330Driver>(port_);
    velocity_commands_.reserve(info_.joints.size());
    position_states_.reserve(info_.joints.size());
    velocity_states_.reserve(info_.joints.size());

    return hardware_interface::CallbackReturn::SUCCESS;
}


hardware_interface::CallbackReturn MobileBaseHardwareInterface::on_configure
    (const rclcpp_lifecycle::State & previous_state)
{
    (void)previous_state;//TO Shut up the compiler from complaining as unused variable.
    if (driver_->init() !=0) {
        return hardware_interface::CallbackReturn::ERROR;
    }

    // for (const auto & [name, descr] : joint_command_interfaces_)
    // {
    //     RCLCPP_INFO(rclcpp::get_logger(), "COMMAND INTERFACE NAME: ");
    //     RCLCPP_INFO(rclcpp::get_logger(), name.c_str());
    // }
    // for (const auto & [name, descr] : joint_state_interfaces_)
    // {
    //     RCLCPP_INFO(rclcpp::get_logger(), "STATE INTERFACE NAME: ");
    //     RCLCPP_INFO(rclcpp::get_logger(), name.c_str());
    // }

    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn MobileBaseHardwareInterface::on_activate
    (const rclcpp_lifecycle::State & previous_state)
{
    (void)previous_state;
    velocity_commands_ = { 0.0, 0.0 };
    position_states_ = { 0.0, 0.0 };
    velocity_states_ = { 0.0, 0.0 };
    /* Jazzy 
    set_state("base_left_wheel_joint/velocity", 0.0);
    set_state("base_right_wheel_joint/velocity", 0.0);
    set_state("base_left_wheel_joint/position", 0.0);
    set_state("base_right_wheel_joint/position", 0.0);
    */
    driver_->activateWithVelocityMode(left_motor_id_);
    driver_->activateWithVelocityMode(right_motor_id_);

    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn MobileBaseHardwareInterface::on_deactivate
    (const rclcpp_lifecycle::State & previous_state)
{
    (void)previous_state;
    driver_->deactivate(left_motor_id_);
    driver_->deactivate(right_motor_id_);
    return hardware_interface::CallbackReturn::SUCCESS;
}

std::vector<hardware_interface::StateInterface> MobileBaseHardwareInterface::export_state_interfaces()
{
  std::vector<hardware_interface::StateInterface> state_interfaces;

  // Provide only a position Interafce
  for (size_t i = 0; i < info_.joints.size(); i++)
  {
    state_interfaces.emplace_back(hardware_interface::StateInterface(
        info_.joints[i].name, hardware_interface::HW_IF_POSITION, &position_states_[i]));
    state_interfaces.emplace_back(hardware_interface::StateInterface(
        info_.joints[i].name, hardware_interface::HW_IF_VELOCITY, &velocity_states_[i]));
  }

  return state_interfaces;
}


std::vector<hardware_interface::CommandInterface> MobileBaseHardwareInterface::export_command_interfaces()
{
  std::vector<hardware_interface::CommandInterface> command_interfaces;
  // Provide only a velocity Interafce
  for (size_t i = 0; i < info_.joints.size(); i++)
  {
    command_interfaces.emplace_back(hardware_interface::CommandInterface(
        info_.joints[i].name, hardware_interface::HW_IF_VELOCITY, &velocity_commands_[i]));
  }

 
  return command_interfaces;
}


hardware_interface::return_type MobileBaseHardwareInterface::read
    (const rclcpp::Time & time, const rclcpp::Duration & period)
{
    (void)time;
    double left_vel = driver_->getVelocityRadianPerSec(left_motor_id_);
    double right_vel = -1.0 * driver_->getVelocityRadianPerSec(right_motor_id_);
    if (abs(left_vel) < 0.03) { left_vel = 0.0; }
    if (abs(right_vel) < 0.03) { right_vel = 0.0; }
    velocity_states_[left_motor_index_] = left_vel; //base_left_wheel_joint/velocity
    velocity_states_[right_motor_index_] = right_vel; //base_right_wheel_joint/velocity
    position_states_[left_motor_index_] += left_vel * period.seconds();
    position_states_[right_motor_index_] += right_vel * period.seconds();
    /* Jazzy 
    // Explanation: set_state("name of interface") where name_of_interface is from mobile_base.ros2_control.xacro 
    set_state("base_left_wheel_joint/velocity", left_vel);
    set_state("base_right_wheel_joint/velocity", right_vel);
    set_state("base_left_wheel_joint/position", get_state("base_left_wheel_joint/position") + left_vel * period.seconds());
    set_state("base_right_wheel_joint/position", get_state("base_right_wheel_joint/position") + right_vel * period.seconds());
     */
    return hardware_interface::return_type::OK;
}

hardware_interface::return_type MobileBaseHardwareInterface::write
    (const rclcpp::Time & time, const rclcpp::Duration & period)
{
    (void)time;
    (void)period;
    //driver_->setTargetVelocityRadianPerSec(left_motor_id_, 3.14159);
    //driver_->setTargetVelocityRadianPerSec(right_motor_id_, -3.14159);
    RCLCPP_INFO(rclcpp::get_logger("MobileBaseHardwareInterface"), "left vel: %lf, right vel: %lf",
        velocity_commands_.at(left_motor_index_),velocity_commands_.at(right_motor_index_));
    driver_->setTargetVelocityRadianPerSec(left_motor_id_,velocity_commands_.at(left_motor_index_));
    driver_->setTargetVelocityRadianPerSec(right_motor_id_, -1.0*velocity_commands_.at(right_motor_index_));
    /* Jazzy 
    driver_->setTargetVelocityRadianPerSec(left_motor_id_, get_command("base_left_wheel_joint/velocity"));
    driver_->setTargetVelocityRadianPerSec(right_motor_id_, -1.0 * get_command("base_right_wheel_joint/velocity"));
    // RCLCPP_INFO(rclcpp::get_logger(), "left vel: %lf, right vel: %lf", get_command("base_left_wheel_joint/velocity"), 
    //                     get_command("base_right_wheel_joint/velocity"));
    */
    //RCLCPP_INFO(rclcpp::get_logger("MobileBaseHardwareInterface"), "left vel: %lf, right vel: %lf",velocity_states_[left_motor_index_] , velocity_states_[right_motor_index_] );
    return hardware_interface::return_type::OK;
}

} // namespace mobile_base_hardware

#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(mobile_base_hardware::MobileBaseHardwareInterface, hardware_interface::SystemInterface)
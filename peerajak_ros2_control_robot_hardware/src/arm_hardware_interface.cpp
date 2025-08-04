#include "peerajak_ros2_control_robot_hardware/arm_hardware_interface.hpp"
#include <hardware_interface/types/hardware_interface_type_values.hpp>
#include <pluginlib/class_list_macros.hpp>


namespace arm_hardware {

hardware_interface::CallbackReturn ArmHardwareInterface::on_init
    (const hardware_interface::HardwareInfo & hardware_info)
{
    if (hardware_interface::SystemInterface::on_init(hardware_info) !=
        hardware_interface::CallbackReturn::SUCCESS)
    {
        return hardware_interface::CallbackReturn::ERROR;
    }

    //info_ = info;
    RCLCPP_INFO(rclcpp::get_logger("ArmHardwareInterface"), info_.hardware_parameters["first_motor_id"].c_str());
    first_motor_id_ = std::stoi(info_.hardware_parameters["first_motor_id"]);
    second_motor_id_ = std::stoi(info_.hardware_parameters["second_motor_id"]);
    port_ = info_.hardware_parameters["dynamixel_port"];
    driver_ = std::make_shared<XL330Driver>(port_);
    position_commands_.reserve(info_.joints.size());
    position_states_.reserve(info_.joints.size());


    return hardware_interface::CallbackReturn::SUCCESS;
}


hardware_interface::CallbackReturn ArmHardwareInterface::on_configure
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

hardware_interface::CallbackReturn ArmHardwareInterface::on_activate
    (const rclcpp_lifecycle::State & previous_state)
{
    (void)previous_state;
    position_commands_ = { 0.0, 0.0 };
    position_states_ = { 0.0, 0.0 };


    driver_->activateWithPositionMode(first_motor_id_);
    driver_->activateWithPositionMode(second_motor_id_);

    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn ArmHardwareInterface::on_deactivate
    (const rclcpp_lifecycle::State & previous_state)
{
    (void)previous_state;
    driver_->deactivate(first_motor_id_);
    driver_->deactivate(second_motor_id_);
    return hardware_interface::CallbackReturn::SUCCESS;
}

std::vector<hardware_interface::StateInterface> ArmHardwareInterface::export_state_interfaces()
{
  std::vector<hardware_interface::StateInterface> state_interfaces;

  // Provide only a position Interafce
  for (size_t i = 0; i < info_.joints.size(); i++)
  {
    state_interfaces.emplace_back(hardware_interface::StateInterface(
        info_.joints[i].name, hardware_interface::HW_IF_POSITION, &position_states_[i]));
    RCLCPP_INFO(rclcpp::get_logger("ArmHardwareInterface"), info_.joints[i].name.c_str());
  }

  return state_interfaces;
}


std::vector<hardware_interface::CommandInterface> ArmHardwareInterface::export_command_interfaces()
{
  std::vector<hardware_interface::CommandInterface> command_interfaces;
  // Provide only a position Interafce
  for (size_t i = 0; i < info_.joints.size(); i++)
  {
    command_interfaces.emplace_back(hardware_interface::CommandInterface(
        info_.joints[i].name, hardware_interface::HW_IF_POSITION, &position_commands_[i]));
    RCLCPP_INFO(rclcpp::get_logger("ArmHardwareInterface"), info_.joints[i].name.c_str());
  }

 
  return command_interfaces;
}


hardware_interface::return_type ArmHardwareInterface::read
    (const rclcpp::Time & time, const rclcpp::Duration & period)
{
    (void)time;
    double first_pos = driver_->getPositionRadian(first_motor_id_);
    double second_pos =  driver_->getPositionRadian(second_motor_id_);


    position_states_[first_motor_index_] = first_pos;
    position_states_[second_motor_index_] = second_pos;

    return hardware_interface::return_type::OK;
}

hardware_interface::return_type ArmHardwareInterface::write
    (const rclcpp::Time & time, const rclcpp::Duration & period)
{
    (void)time;
    (void)period;
    //driver_->setTargetPositionRadian(first_motor_id_, 3.14159);
    //driver_->setTargetPositionRadian(second_motor_id_, -3.14159);
    // RCLCPP_INFO(rclcpp::get_logger("ArmHardwareInterface"), "first vel: %lf, second vel: %lf",
    //     position_commands_.at(first_motor_index_),position_commands_.at(second_motor_index_));
    driver_->setTargetPositionRadian(first_motor_id_,position_commands_.at(first_motor_index_));
    driver_->setTargetPositionRadian(second_motor_id_,position_commands_.at(second_motor_index_));

    //RCLCPP_INFO(rclcpp::get_logger("ArmHardwareInterface"), "first vel: %lf, second vel: %lf",position_states_[first_motor_index_] , position_states_[second_motor_index_] );
    return hardware_interface::return_type::OK;
}

} // namespace arm_hardware

#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(arm_hardware::ArmHardwareInterface, hardware_interface::SystemInterface)
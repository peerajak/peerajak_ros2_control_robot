#ifndef MOBILE_BASE_HARDWARE_INTERFACE_HPP
#define MOBILE_BASE_HARDWARE_INTERFACE_HPP
#include <rclcpp/rclcpp.hpp>
#include "hardware_interface/system_interface.hpp"
#include "peerajak_ros2_control_robot_hardware/xl330_driver.hpp"
#include <rclcpp_lifecycle/state.hpp>
#include <rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp>
namespace mobile_base_hardware {

class MobileBaseHardwareInterface : public hardware_interface::SystemInterface
{
public:
    // Lifecycle node override
    hardware_interface::CallbackReturn
        on_configure(const rclcpp_lifecycle::State & previous_state) override;
    hardware_interface::CallbackReturn
        on_activate(const rclcpp_lifecycle::State & previous_state) override;
    hardware_interface::CallbackReturn
        on_deactivate(const rclcpp_lifecycle::State & previous_state) override;

    // SystemInterface override
    hardware_interface::CallbackReturn
        on_init(const hardware_interface::HardwareInfo & info) override;
    hardware_interface::return_type
        read(const rclcpp::Time & time, const rclcpp::Duration & period) override;
    hardware_interface::return_type
        write(const rclcpp::Time & time, const rclcpp::Duration & period) override;

    std::vector<hardware_interface::StateInterface> export_state_interfaces() override;
    std::vector<hardware_interface::CommandInterface> export_command_interfaces() override;

private:
    std::shared_ptr<XL330Driver> driver_;
    const int left_motor_index_ = 0;
    const int right_motor_index_ = 1;
    int left_motor_id_;
    int right_motor_id_;
    std::string port_;
    std::vector<double> velocity_commands_;
    //double velocity_command_left_, velocity_command_right_;
    std::vector<double> position_states_;
    std::vector<double> velocity_states_;

}; // class MobileBaseHardwareInterface

} // namespace mobile_base_hardware


#endif
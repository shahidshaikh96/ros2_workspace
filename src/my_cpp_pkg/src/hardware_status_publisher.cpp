#include "rclcpp/rclcpp.hpp"
#include "my_robot_interfaces/msg/hardware_status.hpp"

class HardwareStatusPublisher : public rclcpp::Node   
{
public:
  HardwareStatusPublisher() : Node("hardware_status_publisher")   
  {
    publisher_ = this->create_publisher<my_robot_interfaces::msg::HardwareStatus>("hardware_status", 10);   
    timer_ = this->create_wall_timer(
      std::chrono::seconds(1),
      std::bind(&HardwareStatusPublisher::publish_hardware_status, this));
    RCLCPP_INFO(this->get_logger(), "C++ ROS 2 node has started");
  }

private:
  void publish_hardware_status()
    {   
      auto message = my_robot_interfaces::msg::HardwareStatus();
      message.temperature = 42.0;
      message.are_motors_ready = true;
      message.debug_message = "All systems operational";
      publisher_->publish(message);
      RCLCPP_INFO(this->get_logger(), "Published hardware status: '%s'", message.debug_message.c_str());
    }

  rclcpp::Publisher<my_robot_interfaces::msg::HardwareStatus>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<HardwareStatusPublisher>());
  rclcpp::shutdown();
  return 0;
}


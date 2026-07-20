#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "example_interfaces/msg/string.hpp"
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

class RobotNewsStationNode : public rclcpp::Node
{
public:
  RobotNewsStationNode()
  : Node("robot_news_station"), robot_name_("ROS 2")
  {
    publisher_ = this->create_publisher<example_interfaces::msg::String>("robot_news", 10);
    timer_ = this->create_wall_timer(
      500ms, std::bind(&RobotNewsStationNode::publish_news, this));
    RCLCPP_INFO(this->get_logger(), "Robot news station has started");
  }

private:
  void publish_news()
  {
    auto message = example_interfaces::msg::String();
    message.data = "Breaking news from " + robot_name_ + ": ROS 2 is awesome!";
    publisher_->publish(message);
  }

  std::string robot_name_;
  rclcpp::Publisher<example_interfaces::msg::String>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<RobotNewsStationNode>());
  rclcpp::shutdown();
  return 0;
}

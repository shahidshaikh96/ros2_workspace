#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/add_two_ints.hpp"


int main(int argc, char * argv[])

{

  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("add_two_ints_client_no_oop");
  auto client = node->create_client<example_interfaces::srv::AddTwoInts>("add_two_ints");
  while (!client->wait_for_service(std::chrono::seconds(1))) 
  {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(node->get_logger(), "waiting for the server....");
      return 0;
    }
    RCLCPP_INFO(node->get_logger(), "Service not available, waiting again...");
  }
  auto request = std::make_shared<example_interfaces::srv::AddTwoInts::Request>();
  request->a = 6;
  request->b = 2;

  client->async_send_request(request, [node](rclcpp::Client<example_interfaces::srv::AddTwoInts>::SharedFuture future) {
    auto response = future.get();
    RCLCPP_INFO(node->get_logger(), "Result of add_two_ints: %ld", response->sum);
  });
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}


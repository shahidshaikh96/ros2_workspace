#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/add_two_ints.hpp"
#include <memory>

using namespace std::chrono_literals;

class AddTwoIntsClientNode : public rclcpp::Node
{
public:
  AddTwoIntsClientNode()
  : Node("add_two_ints_client")
  {
    client_ = this->create_client<example_interfaces::srv::AddTwoInts>("add_two_ints");
  }

  void callAddTwoInts(int a, int b)
  {
    auto request = std::make_shared<example_interfaces::srv::AddTwoInts::Request>();
    request->a = a;
    request->b = b;

    while (!client_->wait_for_service(1s)) {
      if (!rclcpp::ok()) {
        RCLCPP_ERROR(this->get_logger(), "Interrupted while waiting for the service. Exiting.");
        return;
      }
      RCLCPP_INFO(this->get_logger(), "Service not available, waiting again...");
    }

    auto result_future = client_->async_send_request(request);

    // Wait for the result
    if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), result_future) ==
        rclcpp::FutureReturnCode::SUCCESS)
    {
      auto response = result_future.get();
      RCLCPP_INFO(this->get_logger(), "Result of add_two_ints: %ld", response->sum);
    } else {
      RCLCPP_ERROR(this->get_logger(), "Failed to call service add_two_ints");
    }
  }

private:
  rclcpp::Client<example_interfaces::srv::AddTwoInts>::SharedPtr client_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<AddTwoIntsClientNode>();
  node->callAddTwoInts(6, 2);
  rclcpp::shutdown();
  return 0;
}


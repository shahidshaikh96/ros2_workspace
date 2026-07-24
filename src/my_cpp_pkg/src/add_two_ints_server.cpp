#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/add_two_ints.hpp"

class AddTwoIntsServerNode : public rclcpp::Node
{
public:
  AddTwoIntsServerNode()
  : Node("add_two_ints_server")
  {
    service_ = create_service<example_interfaces::srv::AddTwoInts>(
      "add_two_ints", std::bind(&AddTwoIntsServerNode::callbackAddTwoInts, this, std::placeholders::_1, std::placeholders::_2));
  }

private:
  void callbackAddTwoInts(const example_interfaces::srv::AddTwoInts::Request::SharedPtr request,
                          const example_interfaces::srv::AddTwoInts::Response::SharedPtr response)
  {
    response->sum = request->a + request->b;
    RCLCPP_INFO(this->get_logger(), "Incoming request: a=%ld, b=%ld", request->a, request->b);
    RCLCPP_INFO(this->get_logger(), "Sending back response: [%ld]", response->sum);
  }

  rclcpp::Service<example_interfaces::srv::AddTwoInts>::SharedPtr service_;
};


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<AddTwoIntsServerNode>());
  rclcpp::shutdown();
  return 0;
}
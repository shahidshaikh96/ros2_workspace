#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int64.hpp"

class NumberCounter : public rclcpp::Node
{
public:
    NumberCounter() : Node("number_counter"), count_(0)
    {
        subscriber_ = this->create_subscription<std_msgs::msg::Int64>(
            "number",
            10,
            std::bind(&NumberCounter::numberCallback, this, std::placeholders::_1));

        RCLCPP_INFO(this->get_logger(), "Number Counter Node Started");
    }

private:
    void numberCallback(const std_msgs::msg::Int64::SharedPtr msg)
    {
        count_++;

        RCLCPP_INFO(
            this->get_logger(),
            "Received Number: %ld | Total Messages Received: %d",
            msg->data,
            count_);
    }

    rclcpp::Subscription<std_msgs::msg::Int64>::SharedPtr subscriber_;
    int count_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    rclcpp::spin(std::make_shared<NumberCounter>());

    rclcpp::shutdown();
    return 0;
}
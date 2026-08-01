#include "rclcpp/rclcpp.hpp"
#include "turtlesim/srv/spawn.hpp"
#include "std_msgs/msg/string.hpp"

#include <chrono>
#include <random>
#include <vector>
#include <string>

using namespace std::chrono_literals;

class TurtleSpawner : public rclcpp::Node
{
public:
    TurtleSpawner()
    : Node("turtle_spawner"),
      gen_(rd_()),
      dist_(1.0, 10.0)
    {
        RCLCPP_INFO(this->get_logger(), "Turtle Spawner Node Started!");

        spawn_client_ = this->create_client<turtlesim::srv::Spawn>("spawn");

        while (!spawn_client_->wait_for_service(1s))
        {
            RCLCPP_WARN(this->get_logger(), "Waiting for Spawn Service...");
        }

        RCLCPP_INFO(this->get_logger(), "Connected to Spawn Service!");
        target_killed_sub_ =
            this->create_subscription<std_msgs::msg::String>(
                "/target_killed",
                10,
                std::bind(
                    &TurtleSpawner::targetKilledCallback,
                    this,
                    std::placeholders::_1
        )
        timerCallback();
    );
    );

        
    }   

private:
    void timerCallback()
    {
        auto request = std::make_shared<turtlesim::srv::Spawn::Request>();

        request->x = dist_(gen_);
        request->y = dist_(gen_);
        request->theta = 0.0;
        request->name = "";

        spawn_client_->async_send_request(
            request,
            std::bind(
                &TurtleSpawner::callbackSpawnTurtle,
                this,
                std::placeholders::_1
            )
        );
    }

    void callbackSpawnTurtle(
        rclcpp::Client<turtlesim::srv::Spawn>::SharedFuture future)
    {
        auto response = future.get();

        alive_turtles_.push_back(response->name);
        std_msgs::msg::String msg;
        msg.data = response->name;

        target_turtle_pub_->publish(msg);
        RCLCPP_INFO(this->get_logger(), "Published target: %s", msg.data.c_str());

        RCLCPP_INFO(
            this->get_logger(),
            "Spawned turtle: %s",
            response->name.c_str());
    }
    void targetKilledCallback(
    const std_msgs::msg::String::SharedPtr msg)
{
    RCLCPP_INFO(
        this->get_logger(),
        "Target killed: %s",
        msg->data.c_str()
    );
}

    rclcpp::TimerBase::SharedPtr timer_;

    rclcpp::Client<turtlesim::srv::Spawn>::SharedPtr spawn_client_;

    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr target_turtle_pub_;

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr target_killed_sub_;
   
    std::random_device rd_;

    std::mt19937 gen_;

    std::uniform_real_distribution<double> dist_;

    std::vector<std::string> alive_turtles_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<TurtleSpawner>();

    rclcpp::spin(node);

    rclcpp::shutdown();
    return 0;
}
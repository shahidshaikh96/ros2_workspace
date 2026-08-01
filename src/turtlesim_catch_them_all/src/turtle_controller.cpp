#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/string.hpp"
#include "turtlesim/msg/pose.hpp"
#include "turtlesim/srv/kill.hpp"

#include <cmath>
#include <string>

using namespace std::chrono_literals;

class TurtleController : public rclcpp::Node
{
public:
    TurtleController()
    : Node("turtle_controller"),
      target_selected_(false),
      has_turtle_pose_(false),
      has_target_pose_(false),
      kill_in_progress_(false)
    {
        RCLCPP_INFO(this->get_logger(), "Turtle Controller Node Started!");

        cmd_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>(
            "/turtle1/cmd_vel",
            10
        );

        target_killed_pub_ = this->create_publisher<std_msgs::msg::String>(
            "/target_killed",
            10
        );

        kill_client_ = this->create_client<turtlesim::srv::Kill>("kill");
        while (!kill_client_->wait_for_service(1s)) {
            RCLCPP_WARN(this->get_logger(), "Waiting for Kill Service...");
        }

        pose_sub_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose",
            10,
            std::bind(
                &TurtleController::turtlePoseCallback,
                this,
                std::placeholders::_1
            )
        );

        target_turtle_sub_ = this->create_subscription<std_msgs::msg::String>(
            "/target_turtle",
            10,
            std::bind(
                &TurtleController::targetTurtleCallback,
                this,
                std::placeholders::_1
            )
        );

        control_timer_ = this->create_wall_timer(
            50ms,
            std::bind(
                &TurtleController::controlLoop,
                this
            )
        );
    }

private:
    void turtlePoseCallback(const turtlesim::msg::Pose::SharedPtr msg)
    {
        turtle_pose_ = *msg;
        has_turtle_pose_ = true;
    }

    void targetPoseCallback(const turtlesim::msg::Pose::SharedPtr msg)
    {
        target_pose_ = *msg;
        has_target_pose_ = true;
    }

    void targetTurtleCallback(const std_msgs::msg::String::SharedPtr msg)
    {
        if (!msg || msg->data.empty()) {
            return;
        }

        if (msg->data == current_target_name_) {
            return;
        }

        current_target_name_ = msg->data;
        target_selected_ = true;
        has_target_pose_ = false;
        kill_in_progress_ = false;

        if (target_pose_sub_) {
            target_pose_sub_.reset();
        }

        target_pose_sub_ = this->create_subscription<turtlesim::msg::Pose>(
            "/" + current_target_name_ + "/pose",
            10,
            std::bind(
                &TurtleController::targetPoseCallback,
                this,
                std::placeholders::_1
            )
        );

        RCLCPP_INFO(
            this->get_logger(),
            "New target received: %s",
            current_target_name_.c_str()
        );
    }

    void controlLoop()
    {
        if (!target_selected_ || kill_in_progress_ || !has_turtle_pose_ || !has_target_pose_) {
            return;
        }

        double dx = target_pose_.x - turtle_pose_.x;
        double dy = target_pose_.y - turtle_pose_.y;
        double distance = std::sqrt(dx * dx + dy * dy);
        double target_angle = std::atan2(dy, dx);
        double angle_error = target_angle - turtle_pose_.theta;

        while (angle_error > M_PI) {
            angle_error -= 2.0 * M_PI;
        }
        while (angle_error < -M_PI) {
            angle_error += 2.0 * M_PI;
        }

        geometry_msgs::msg::Twist cmd;

        if (distance < 0.5) {
            cmd.linear.x = 0.0;
            cmd.angular.z = 0.0;
            cmd_vel_pub_->publish(cmd);

            if (!kill_in_progress_) {
                kill_in_progress_ = true;
                sendKillRequest();
            }
            return;
        }

        cmd.linear.x = std::min(2.0, distance);
        cmd.angular.z = 4.0 * angle_error;
        cmd_vel_pub_->publish(cmd);
    }

    void sendKillRequest()
    {
        auto request = std::make_shared<turtlesim::srv::Kill::Request>();
        request->name = current_target_name_;

        kill_client_->async_send_request(
            request,
            std::bind(
                &TurtleController::onKillResponse,
                this,
                std::placeholders::_1
            )
        );
    }

    void onKillResponse(rclcpp::Client<turtlesim::srv::Kill>::SharedFuture future)
    {
        (void)future;

        std_msgs::msg::String msg;
        msg.data = current_target_name_;
        target_killed_pub_->publish(msg);

        RCLCPP_INFO(
            this->get_logger(),
            "Published target_killed for %s",
            current_target_name_.c_str()
        );

        current_target_name_.clear();
        target_selected_ = false;
        has_target_pose_ = false;
        kill_in_progress_ = false;

        if (target_pose_sub_) {
            target_pose_sub_.reset();
        }
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr target_killed_pub_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_sub_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr target_pose_sub_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr target_turtle_sub_;
    rclcpp::Client<turtlesim::srv::Kill>::SharedPtr kill_client_;
    rclcpp::TimerBase::SharedPtr control_timer_;

    turtlesim::msg::Pose turtle_pose_; 
    turtlesim::msg::Pose target_pose_;

    std::string current_target_name_;
    bool target_selected_;
    bool has_turtle_pose_;
    bool has_target_pose_;
    bool kill_in_progress_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleController>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}

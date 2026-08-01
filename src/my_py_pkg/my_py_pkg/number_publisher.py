#!/usr/bin/env python3

from rclpy.parameter import Parameter
import rclpy
from rclpy.node import Node
from std_msgs.msg import Int64


class NumberPublisher(Node):

    def __init__(self):
        super().__init__("number_publisher")
        self.declare_parameter("number", 2)
        self.declare_parameter("timer_period", 1.0)
        self.number_ = self.get_parameter("number").value
        self.timer_period_ = self.get_parameter("timer_period").value 
        self.add_post_set_parameters_callback(self.parameter_callback)

        # Publish on the "number" topic
        self.publisher_ = self.create_publisher(
            Int64,
            "number",
            10
        )

        self.timer_ = self.create_timer(1.0, self.publish_number)

        self.count_ = 0

        self.get_logger().info("Number Publisher node has been started.")

    def publish_number(self):
        msg = Int64()
        msg.data = self.count_

        self.publisher_.publish(msg)

        self.get_logger().info(f"Publishing: {msg.data}")
    

        self.count_ += 1
    def parameter_callback(self, params: list[]):
        for param in params:
            if param.name == "number":
                self.number_ = param.value
                self.get_logger().info(f"Parameter 'number' changed to: {self.number_}")
            elif param.name == "timer_period":
                self.timer_period_ = param.value
                self.timer_.cancel()  # Cancel the existing timer
                self.timer_ = self.create_timer(self.timer_period_, self.publish_number)  # Create a new timer with the updated period
                self.get_logger().info(f"Parameter 'timer_period' changed to: {self.timer_period_}")
        return rclpy.parameter.ParameterEventDescriptors()

def main(args=None):
    rclpy.init(args=args)

    node = NumberPublisher()

    rclpy.spin(node)

    node.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()
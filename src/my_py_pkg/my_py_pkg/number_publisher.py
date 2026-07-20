#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from std_msgs.msg import Int64


class number_publisher(Node):
    def __init__(self):
        super().__init__("number_publisher")
        self.publisher_ = self.create_publisher(Int64, "number_count", 10)
        self.timer_ = self.create_timer(1.0, self.publish_number)
        self.count_ = 0

    def publish_number(self):
        msg = Int64()
        msg.data = self.count_
        self.publisher_.publish(msg)
        self.get_logger().info(f"Publishing: {self.count_}")
        self.count_ += 1


def main(args=None):
    rclpy.init(args=args)
    node = number_publisher()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == "__main__":
    main()
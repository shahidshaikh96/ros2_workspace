#!/usr/bin/env python3

import rclpy
from rclpy.node import Node

from my_robot_interfaces.msg import HardwareStatus


class HardwareStatusPublisherNode(Node):

    def __init__(self):
        super().__init__("hardware_status_publisher")

        self.hw_publisher_ = self.create_publisher(
            HardwareStatus,
            "hardware_status",
            10
        )

        self.timer_ = self.create_timer(
            1.0,
            self.publish_hardware_status
        )

    def publish_hardware_status(self):

        msg = HardwareStatus()

        msg.temperature = 63.33
        msg.are_motors_ready = True
        msg.debug_message = "Nothing to report"

        self.hw_publisher_.publish(msg)

        self.get_logger().info(
            f"Temperature: {msg.temperature} °C | "
            f"Motors Ready: {msg.are_motors_ready} | "
            f"Debug: {msg.debug_message}"
        )


def main(args=None):

    rclpy.init(args=args)

    node = HardwareStatusPublisherNode()

    rclpy.spin(node)

    node.destroy_node()

    rclpy.shutdown()


if __name__ == "__main__":
    main()
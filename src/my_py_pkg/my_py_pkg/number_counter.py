#!/usr/bin/env python3

import rclpy
from rclpy.node import Node

from std_msgs.msg import Int64
from example_interfaces.srv import SetBool


class NumberCounter(Node):

    def __init__(self):
        super().__init__("number_counter")

        # Counter variable
        self.count = 0

        # Subscriber
        self.subscription = self.create_subscription(
            Int64,
            "number",
            self.number_callback,
            10
        )

        # Publisher
        self.publisher = self.create_publisher(
            Int64,
            "number_count",
            10
        )

        # Service Server
        self.server = self.create_service(
            SetBool,
            "reset_counter",
            self.reset_counter_callback
        )

        self.get_logger().info("Number Counter node has been started.")
        self.get_logger().info("Service '/reset_counter' is ready.")

    def number_callback(self, msg):

        # Increment counter
        self.count += 1

        self.get_logger().info(
            f"Received Number: {msg.data}"
        )

        # Publish counter
        out_msg = Int64()
        out_msg.data = self.count

        self.publisher.publish(out_msg)

        self.get_logger().info(
            f"Published Count: {out_msg.data}"
        )

    def reset_counter_callback(self, request, response):

        if request.data:
            self.count = 0

            response.success = True
            response.message = "Counter has been reset to 0."

            self.get_logger().info("Counter reset to 0.")

        else:

            response.success = False
            response.message = "Counter was not reset because data=False."

            self.get_logger().info("Reset request ignored.")

        return response


def main(args=None):

    rclpy.init(args=args)

    node = NumberCounter()

    rclpy.spin(node)

    node.destroy_node()

    rclpy.shutdown()


if __name__ == "__main__":
    main()
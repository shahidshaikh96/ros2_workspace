#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from std_msgs.msg import Int64


class NumberCounter(Node):
    def __init__(self):
        super().__init__('number_counter')

        self.subscriber_ = self.create_subscription(
            Int64,
            'number',
            self.number_callback,
            10
        )

        self.publisher_ = self.create_publisher(
            Int64,
            'number_count',
            10
        )

        self.get_logger().info('NumberCounter node started')

    def number_callback(self, msg):
        self.get_logger().info(f'Received: {msg.data}')

        out_msg = Int64()
        out_msg.data = msg.data
        self.publisher_.publish(out_msg)

        self.get_logger().info(f'Published: {out_msg.data}')


def main(args=None):
    rclpy.init(args=args)
    node = NumberCounter()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()

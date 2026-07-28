#!/usr/bin/env python3

from urllib import response

from my_robot_interfaces import msg
import rclpy
from rclpy.node import Node

from my_robot_interfaces.msg import LedPanelState
from my_robot_interfaces.srv import SetLedPanelState


class LedPanelNode(Node):

    def __init__(self):
        super().__init__("led_panel_node")

        # Publisher
        self.publisher_ = self.create_publisher(
            LedPanelState,
            "led_panel_state",
            10,
        )

        # Timer (publishes every 1 second)
        self.timer_ = self.create_timer(
            1.0,
            self.publish_led_state,
        )

        # Service Server
        self.service_ = self.create_service(
            SetLedPanelState,
            "set_led_panel_state",
            self.set_led_callback,
        )

        # Initial LED State
        self.led1 = False
        self.led2 = False
        self.led3 = False

        self.get_logger().info("LED Panel Node Started")

    def publish_led_state(self):

        msg = LedPanelState()

        msg.led1 = self.led1
        msg.led2 = self.led2
        msg.led3 = self.led3

        self.publisher_.publish(msg)

        self.get_logger().info(
        f"LED State -> [{self.led1}, {self.led2}, {self.led3}]"
)

    def set_led_callback(self, request, response):

        self.led1 = request.led1
        self.led2 = request.led2
        self.led3 = request.led3

        response.success = True
        response.message = "LED panel updated successfully"

        self.get_logger().info(
        f"Service Request -> [{int(self.led1)}, {int(self.led2)}, {int(self.led3)}]"
    )

        return response


def main(args=None):
    rclpy.init(args=args)

    node = LedPanelNode()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
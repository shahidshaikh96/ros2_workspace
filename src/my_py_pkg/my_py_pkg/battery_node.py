#!/usr/bin/env python3

import rclpy
from rclpy.node import Node

from my_robot_interfaces.srv import SetLedPanelState


class BatteryNode(Node):

    def __init__(self):
        super().__init__("battery_node")

        # Service Client
        self.client = self.create_client(
            SetLedPanelState,
            "set_led_panel_state"
        )

        while not self.client.wait_for_service(timeout_sec=1.0):
            self.get_logger().info("Waiting for LED Panel Service...")

        # Initial Battery State
        self.battery_full = True

        self.get_logger().info("Battery FULL")

        # First timer (battery becomes empty after 4 seconds)
        self.timer = self.create_timer(
            4.0,
            self.update_battery
        )

    def update_battery(self):

        request = SetLedPanelState.Request()

        if self.battery_full:

            self.get_logger().info("Battery EMPTY")

            request.led1 = True
            request.led2 = False
            request.led3 = False

            self.battery_full = False

            self.timer.cancel()
            self.timer = self.create_timer(
                6.0,
                self.update_battery
            )

        else:

            self.get_logger().info("Battery FULL")

            request.led1 = False
            request.led2 = False
            request.led3 = False

            self.battery_full = True

            self.timer.cancel()
            self.timer = self.create_timer(
                4.0,
                self.update_battery
            )

        future = self.client.call_async(request)
        future.add_done_callback(self.service_response_callback)

    def service_response_callback(self, future):

        try:
            response = future.result()

            self.get_logger().info(
                f"Service Response: {response.message}"
            )

        except Exception as e:
            self.get_logger().error(str(e))


def main(args=None):

    rclpy.init(args=args)

    node = BatteryNode()

    try:
        rclpy.spin(node)

    except KeyboardInterrupt:
        pass

    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from example_interfaces.srv import AddTwoInts

 
 
class AddTwoIntsClient(Node):
    def __init__(self):
        super().__init__("add_two_ints_client")
        self.client = self.create_client(AddTwoInts, "add_two_ints")

    def call_add_two_ints(self, a: int, b: int):
        while not self.client.wait_for_service(timeout_sec=1.0):
            self.get_logger().info("waiting for service 'add_two_ints' server...")
        request = AddTwoInts.Request()
        request.a = a
        request.b = b

        future = self.client.call_async(request)
        future.add_done_callback(lambda fut, a=a, b=b: self.callback_call_add_two_ints(fut, a, b))

    def callback_call_add_two_ints(self, future, a, b):
        try:
            response = future.result()
        except Exception as e:
            self.get_logger().error(f"Service call failed: {e}")
            return
        self.get_logger().info(f"Got response: {a} + {b} = {response.sum}")
        
 
 
def main(args=None):
    rclpy.init(args=args)
    node = AddTwoIntsClient() 
    node.call_add_two_ints(5, 10) 
    rclpy.spin(node)
    rclpy.shutdown()
 
 
if __name__ == "__main__":
    main()
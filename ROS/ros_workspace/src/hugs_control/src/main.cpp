#include "hugs_control/hugs.h"
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>


// Declare your variables and functions
int16_t linear_speed = 0;
int16_t angular_speed = 0;
ros::Time last_cmd_time;

void cmdVelCallback(const geometry_msgs::Twist::ConstPtr& msg) {
    ROS_INFO("Received cmd_vel");
    ROS_INFO("Received cmd_vel: linear.x=%f, angular.z=%f", msg->linear.x, msg->angular.z);
    linear_speed = msg->linear.x * 1000.0;
    angular_speed = msg->angular.z * 100.0;
    setTwist(linear_speed, angular_speed);
    last_cmd_time = ros::Time::now();
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "hugs_control");
    ros::NodeHandle nh;
    ros::Time::init();
    ROS_INFO("HUGS control node started.");

    ros::Subscriber sub = nh.subscribe("cmd_vel", 10, cmdVelCallback);


    start_HUGS_tasks();

    //ros::spin();  

    return 0;
}

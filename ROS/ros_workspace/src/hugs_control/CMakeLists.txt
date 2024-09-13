cmake_minimum_required(VERSION 2.8.3)
project(hugs_control)


find_package(catkin REQUIRED COMPONENTS
  roscpp
  serial
  std_msgs
  geometry_msgs
 
)

 
catkin_package(
  INCLUDE_DIRS include/hugs_control
  LIBRARIES hugs_control
  CATKIN_DEPENDS roscpp serial std_msgs
#  DEPENDS system_lib
)
 
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)
 
add_executable(hugs_control_node src/main.cpp src/HUGS.cpp)



 
target_link_libraries(hugs_control_node
  ${catkin_LIBRARIES}
)

#ifndef HUGS_H_
#define HUGS_H_

#include <ros/ros.h>
#include <std_msgs/Int16.h>
#include <geometry_msgs/Twist.h>
#include <vector>
#include <string>
#include <serial/serial.h>
// Command and mode definitions
#define RES  2
#define SMOD 8
#define MOD  10
#define POW  5
#define SPE  6
#define XXX  0xFF
#define SMOT 1


// Constants and definitions
#define SPEED_DUAL 1 // Example value, adjust as necessary
#define CONTROL_LOOP_PERIOD 20 // Control loop period in milliseconds
#define TOP_SPEED 5000 // Define maximum speed for normalization
#define MM_PER_DEGREE 3.508 // Define millimeters per degree

// DRIVE_STATUS structure to hold the status of each drive
struct DRIVE_STATUS {
    std::string name;      // Name of the device (e.g., "Master" or "Slave")
    uint8_t port_num;      // Port number or identifier
    uint8_t response;      // Response from the device
    uint8_t RXbuf[64];     // Buffer to store incoming data
    uint8_t RXIndex;       // Index to track the buffer position
    bool appending;        // Flag to indicate if data is being appended
    uint8_t pending;       // Count of pending messages
    uint8_t state;         // Current state of the device
    int16_t power;         // Power level
    int32_t mm;            // Position in millimeters
    int16_t mmps;          // Speed in millimeters per second
    uint16_t mV;           // Voltage in millivolts
    uint16_t mA;           // Current in milliamps
    uint16_t wdog;         // Watchdog timer value
    uint8_t controlMode;   // Current control mode
    uint8_t speedMode;     // Current speed mode
    uint8_t maxStepSpeed;  // Maximum step speed
    int16_t outF;          // Output forward value
    int16_t outP;          // Output position value
    int16_t outI;          // Output integral value
};

// HUGS class declaration
class HUGS {
public:
    HUGS();  // Constructor
    void init_HUGS(); // Initialize the HUGS system
    void start_HUGS_tasks(ros::NodeHandle& nh); // Start HUGS tasks
    void resetPosition(); // Reset the position (e.g., encoders)
    void setSpeedMode(uint8_t mode, uint8_t maxStepSpeed); // Set the speed mode
    void setSpeeds(int16_t newLeftSpeed, int16_t newRightSpeed); // Set speeds for left and right wheels
    void setTwist(int16_t mmPerSec, int16_t degreesPerSec); // Set twist (linear and angular velocities)
    void shutDown(const std::string& reason); // Handle shutdown
    int sendData(DRIVE_STATUS *status, uint8_t *buffer, int len);
    void checkRXBuffer(DRIVE_STATUS* status); // Check the received data buffer
    void setupPort(DRIVE_STATUS* status, const std::string& name, int portNum);

private:
    // Utility functions for CRC and data conversion
    uint16_t CalcCRC(uint8_t* ptr);
    int8_t bytesToInt8(uint8_t* ndata);
    int16_t bytesToInt16(uint8_t* ndata);
    uint16_t bytesToUInt16(uint8_t* ndata);
    int32_t bytesToInt32(uint8_t* ndata);
    void int16ToBytes(int16_t num, uint8_t* ndata);

    // ROS-specific callbacks and tasks
    void teleopCallback(const geometry_msgs::Twist::ConstPtr& msg); // Callback for teleop commands
    void txTaskLoop(const ros::TimerEvent&); // Task loop for sending commands
    void rxTaskLoop(DRIVE_STATUS* status); // Task loop for receiving data

    // Data members
    DRIVE_STATUS master; // Master drive status
    DRIVE_STATUS slave;  // Slave drive status

    int16_t leftSpeed;  // Left wheel speed
    int16_t rightSpeed; // Right wheel speed

    ros::Publisher data_pub;  // ROS publisher for sending data
    ros::Subscriber cmd_vel_sub;  // ROS subscriber for receiving teleop commands
    ros::Timer tx_timer;  // ROS timer for periodic tasks

    bool shuttingDown;  // Flag to indicate if system is shutting down
    uint8_t sequence;   // Sequence number for messages

    // Serial objects for communication
    serial::Serial master_serial;
    serial::Serial slave_serial;

    // Command buffers
    uint8_t REScommand[8];
    uint8_t MODcommand[9];
    uint8_t PWRcommand[9];
    uint8_t SPEcommand[9];
    uint8_t XXXcommand[8];
};

#endif /* HUGS_H_ */


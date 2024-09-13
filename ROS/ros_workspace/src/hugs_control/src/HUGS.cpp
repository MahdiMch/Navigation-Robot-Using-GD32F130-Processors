#include "hugs_control/hugs.h"
#include "serial/serial.h"
#include "cmath"
#include <ros/ros.h>
//#include <hugs_control/RobotStatus.h>

ros::Publisher status_pub;




static const char *TAG = "hugs";
static const bool verbose  = true;
static const bool xverbose = false;

DRIVE_STATUS master;
DRIVE_STATUS slave;

serial::Serial Master;
serial::Serial Slave;

static const bool enable_buffer_rx = false;      



int16_t leftSpeed = 0;
int16_t rightSpeed = 0;

uint8_t REScommand[]    = {'/', 0,  0, RES, SMOD,             0,   0, '\n'};
uint8_t MODcommand[]    = {'/', 2,  0, MOD, SMOD,   SPEED_DUAL, 250, 0,   0, '\n'};
uint8_t PWRcommand[]    = {'/', 2,  0, POW, SMOT,   0, 0,     0,   0, '\n'};
uint8_t SPEcommand[]    = {'/', 2,  0, SPE, SMOT,   0, 0,     0,   0, '\n'};
uint8_t XXXcommand[]    = {'/', 0,  0, XXX, SMOT,             0,   0, '\n'};

bool shuttingDown = false;
uint8_t sequence =   0;

int max(int a, int b) {
return ((a > b) ? a : b);
}

void int16ToBytes(int16_t num, uint8_t * ndata) ;

void setSpeeds(uint16_t newLeftSpeed, int16_t newRightSpeed) {
leftSpeed  = newLeftSpeed;
rightSpeed = newRightSpeed;
        //ROS_INFO("Setting speeds: left=%d, right=%d", leftSpeed, rightSpeed);

}

void setTwist(int16_t mmPerSec,  int16_t degreesPerSec) {
       
int leftDrive  = mmPerSec ;
int rightDrive = -mmPerSec ;
int turnSpeed  = (int)((float)degreesPerSec * MM_PER_DEGREE);
//printf("MM_PER_DEGREE %02X ",(float)MM_PER_DEGREE);
        //printf("degreesPerSec %02X ",(float)degreesPerSec);
int smax = 0;

leftDrive  += turnSpeed;
rightDrive += turnSpeed;

smax = max(abs(leftDrive), abs(rightDrive)); //TODO

if (smax > TOP_SPEED) {
leftDrive  = (leftDrive  * TOP_SPEED) / smax;
rightDrive = (rightDrive * TOP_SPEED) / smax;
}
setSpeeds(leftDrive, rightDrive);
}
void rx_task(DRIVE_STATUS* status) {
    uint8_t abyte = 0;

    if (Master.available()) {              
        abyte = Master.read(1)[0];        

        if (!status->appending && abyte == '/') {
            status->appending = true;          
        }

        if (status->appending) {                
            status->RXbuf[status->RXIndex++] = abyte;
            if ((status->RXIndex > 2) && (status->RXIndex == status->RXbuf[1] + 8)) {
                if (enable_buffer_rx) {        
                    ROS_INFO("Received message: ");
                    for (int i = 0; i < status->RXIndex; i++) {
                        ROS_INFO("%02X ", status->RXbuf[i]);
                    }
                }
                checkRXBuffer(status);          
                status->RXIndex = 0;            
                status->appending = false;      
            }
        }
    }
}


void start_HUGS_tasks()
{
        serial::Timeout to = serial::Timeout::simpleTimeout(1000);
try {


        printf("Start Serial");
        Slave.setPort("/dev/ttyUSB1");
        Slave.setBaudrate(115200);

Slave.setTimeout(to);
        Slave.open();
        Master.setPort("/dev/ttyUSB0");
        Master.setBaudrate(115200);
        Master.setTimeout(to);
        Master.open();


       
} catch (serial::IOException& e) {
        ROS_ERROR("Unable to open port");
}

tx_task();

     
}

       

 
void tx_task()
{
    ros::Rate rate(10);  
     ROS_INFO("open port");
   
    while (ros::ok())  
    {
//int16_t speed=10;
//int16_t degree=100;
//setTwist(speed,degree);
        SPEcommand[4] = SMOT;
        int16ToBytes(leftSpeed, &SPEcommand[5]);
        sendData(&master, SPEcommand, sizeof(SPEcommand));
        //printf("leftspeed %d ",leftSpeed);

        int16ToBytes(rightSpeed, &SPEcommand[5]);
        sendData(&slave, SPEcommand, sizeof(SPEcommand));
        //printf("rightspeed %d ",rightSpeed);

        sequence += 0x10;

        rate.sleep();  // Sleep for the duration of the rate cycle
  ros::spinOnce();
    }


 
}
 


void checkRXBuffer(DRIVE_STATUS * status) {
    uint8_t length  = status->RXbuf[1] + 5;
    uint16_t crc = CalcCRC(status->RXbuf);
}



int sendData(DRIVE_STATUS * status, uint8_t * buffer, int len)
{
// Add CRC to message
uint8_t dataLength;
    uint16_t crc;

dataLength = buffer[1];
    buffer[2] = sequence;
    crc = CalcCRC((uint8_t *)buffer);

    buffer[dataLength + 5] = crc & 0xFF;
    buffer[dataLength + 6] = (crc >> 8) & 0xFF;
    buffer[dataLength + 7] = '\n';

    if (status->pending < MAX_PENDING) {
    status->pending++;
    }

    for (int i = 0; i < buffer[1] + 8; i++) {
if (status == &master){
      Master.write(&buffer[i], 1);
}else if (status == &slave){
      Slave.write(&buffer[i], 1);

     // printf("Sent byte %02X to Master UART: %02X\n", (int)buffer[i], (int)buffer[i]);

 }
       
    }

    return 0;




}


 

//----------------------------------------------------------------------------
// Calculate CRC
//----------------------------------------------------------------------------
uint16_t CalcCRC(uint8_t *ptr)
{
  uint16_t  crc;
  uint8_t i;
  int8_t count = ptr[1] + 5;
  crc = 0;

  while (--count >= 0)
  {
    crc = crc ^ (uint16_t) *ptr++ << 8;
    i = 8;
    do
    {
      if (crc & 0x8000)
      {
        crc = crc << 1 ^ 0x1021;
      }
      else
      {
        crc = crc << 1;
      }
    } while(--i);
  }
  return (crc);
}

int8_t bytesToInt8 (uint8_t * ndata) {
return (int8_t)ndata[6] ;
}

int16_t bytesToInt16 (uint8_t * ndata) {
return (int16_t)(((uint16_t)ndata[7] << 8) + ndata[6]);
}

uint16_t bytesToUInt16 (uint8_t * ndata) {
return (((uint16_t)ndata[7] << 8) + ndata[6]);
}

int32_t bytesToInt32 (uint8_t * ndata) {
return ((int32_t)(((uint32_t)ndata[9] << 24) | ((uint32_t)ndata[8] << 16) | ((uint32_t)ndata[7] << 8) |  ndata[6]));
}

void int16ToBytes(int16_t num, uint8_t * ndata) {
*ndata++ = ((uint8_t)((uint16_t)num & 0xFF));
*ndata   = ((uint8_t)((uint16_t)num >> 8));
}

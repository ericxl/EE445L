#include <stdint.h>
#include "I2C0.h"
#include "Accelerometer.h"

uint16_t XAccel;
uint16_t YAccel;
uint16_t ZAccel;
uint16_t XGyro;
uint16_t YGyro;
uint16_t ZGyro;

void GetAccelerometerData(void){
	XAccel = (I2C_Recv_MPU6050(0x68, 0x3B) << 8) + I2C_Recv_MPU6050(0x68, 0x3C);
	YAccel = (I2C_Recv_MPU6050(0x68, 0x3D) << 8) + I2C_Recv_MPU6050(0x68, 0x3E);
	ZAccel = (I2C_Recv_MPU6050(0x68, 0x3F) << 8) + I2C_Recv_MPU6050(0x68, 0x40);
	
	XGyro = (I2C_Recv_MPU6050(0x68, 0x43) << 8) + I2C_Recv_MPU6050(0x68, 0x44);
	YGyro = (I2C_Recv_MPU6050(0x68, 0x45) << 8) + I2C_Recv_MPU6050(0x68, 0x46);
	ZGyro = (I2C_Recv_MPU6050(0x68, 0x47) << 8) + I2C_Recv_MPU6050(0x68, 0x48);
}

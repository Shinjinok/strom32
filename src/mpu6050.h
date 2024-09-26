/*
 * mpu6050.h
 *
 *  Created on: May 16, 2024
 *      Author: sjo
 */

#ifndef MPU6050_H_
#define MPU6050_H_

#include "ch.h"
#include "hal.h"

/*가속도 값 저장되는 레지스터*/
#define ADDRESS_ACCEL_XOUT_H 0x3B  //ACCEL_XOUT[15:8]
#define MPU6050_ADDR 0x68 //0b1101000 //0x68
//#define MPU6050_ADDR 0b1101000 //0x68
#define WHO_AM_I_REG 0x75
#define PWR_MGMT_1_REG 0x6B
#define SMPLRT_DIV_REG 0x19
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define TEMP_OUT_H_REG 0x41
#define GYRO_CONFIG_REG 0x1B
#define GYRO_XOUT_H_REG 0x43


#define GET_DATA_SIZE 14
typedef  struct //센서값과 단위변환 인자를 멤버로 갖는 구조체
  {
    int16_t acx_raw;   //단위가 중력 가속도 일때, 센서 값 저장되는 변수
    int16_t acy_raw;
    int16_t acz_raw;
    int16_t temp_raw;
    int16_t gyx_raw; //단위가 deg/sec 일때, 센서 값 저장되는 변수
    int16_t gyy_raw;
    int16_t gyz_raw;
    float acx_nomal;
    float acy_nomal;
    float acz_nomal;
    float temp_c;
    float gyx_rps;
    float gyy_rps;
    float gyz_rps;
}MPU6050;

extern MPU6050 mpu6050;
extern uint8_t mpuGetData[GET_DATA_SIZE];
//Initialize
void mpu6050init(void);

#endif /* MPU6050_H_ */

/*
 * mpu6050.c
 *
 *  Created on: May 16, 2024
 *      Author: sjo
 */
#include "ch.h"
#include "hal.h"
#include "mpu6050.h"
#include "chtypes.h"
//#include "i2cdev_chibi.h"
#include "chprintf.h"
#include <string.h>


#define GYRO_FACTOR 0.00763358778f // 1/131
#define ACCEL_FACTOR 0.00006103515f // 1/16384
#define temperature_factor 0.00294117647f // 1/340

MPU6050 mpu6050;

static THD_WORKING_AREA(CalwaThread, 128);
static THD_FUNCTION(CalThread, arg) {

  (void)arg;

  chRegSetThreadName("calculate");
  while (true) {
    mpu6050.acx_nomal = (float) mpu6050.acx_raw * ACCEL_FACTOR;
    mpu6050.acy_nomal = (float) mpu6050.acy_raw * ACCEL_FACTOR;
    mpu6050.acz_nomal = (float) mpu6050.acz_raw * ACCEL_FACTOR;
    mpu6050.gyx_rps = (float) mpu6050.gyx_raw * GYRO_FACTOR;
    mpu6050.gyy_rps = (float) mpu6050.gyy_raw * GYRO_FACTOR;
    mpu6050.gyz_rps = (float) mpu6050.gyz_raw * GYRO_FACTOR;
    chThdSleepMilliseconds(10);
  }
}
void mpu6050init(void){


    msg_t msg;
    uint8_t data_to_write[2];
   // chMtxObjectInit(&mt_mpu6050);
    //uint8_t data_to_write[2]; //레지스터 주소와 그 레지스터에 새로 넣어줄 값를 저장하는 배열
    data_to_write[0] = PWR_MGMT_1_REG;
    data_to_write[1] = 0;

    i2cAcquireBus(&I2CD1);
    msg = i2cMasterTransmitTimeout(&I2CD1,MPU6050_ADDR, data_to_write, 2, NULL, 0, 10);
    i2cReleaseBus(&I2CD1);
    //osalDbgCheck(MSG_OK == msg);

    // Set DATA RATE of 1KHz by writing SMPLRT_DIV register
    data_to_write[0] = SMPLRT_DIV_REG;
    data_to_write[1] = 0x07;

    i2cAcquireBus(&I2CD1);
    msg = i2cMasterTransmitTimeout(&I2CD1,MPU6050_ADDR, data_to_write, 2, NULL, 0, 10);
    i2cReleaseBus(&I2CD1);
    //osalDbgCheck(MSG_OK == msg);
    // Set accelerometer configuration in ACCEL_CONFIG Register
    // XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> � 2g
    data_to_write[0] = ACCEL_CONFIG_REG;
    data_to_write[1] = 0;

    i2cAcquireBus(&I2CD1);
    msg = i2cMasterTransmitTimeout(&I2CD1,MPU6050_ADDR, data_to_write, 2, NULL, 0, 10);
    i2cReleaseBus(&I2CD1);
    //osalDbgCheck(MSG_OK == msg);

    // Set Gyroscopic configuration in GYRO_CONFIG Register
    // XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> � 250 �/s
    data_to_write[0] = GYRO_CONFIG_REG;
    data_to_write[1] = 0;

    i2cAcquireBus(&I2CD1);
    msg = i2cMasterTransmitTimeout(&I2CD1,MPU6050_ADDR, data_to_write, 2, NULL, 0, 10);
    i2cReleaseBus(&I2CD1);
    //osalDbgCheck(MSG_OK == msg);

  //  palSetPadMode(GPIOC, GPIOC_ARD_A5, PAL_MODE_OUTPUT_PUSHPULL);//
  chThdCreateStatic(CalwaThread, sizeof(CalwaThread), NORMALPRIO, CalThread, NULL);

}




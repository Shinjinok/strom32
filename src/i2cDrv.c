/*
 * i2cDrv.c
 *
 *  Created on: Jun 3, 2024
 *      Author: sjo
 */
#include "ch.h"
#include "hal.h"
#include "mpu6050.h"
#include "as5600.h"
#include "chtypes.h"
//#include "i2cdev_chibi.h"
#include "chprintf.h"
#include <string.h>
/* Configure I2C for sensors */
static const I2CConfig i2cfg1 = {
    OPMODE_I2C,
    200000,
    FAST_DUTY_CYCLE_2
};
static uint8_t Buff[32];
static THD_WORKING_AREA(i2cwaThread, 256);
static THD_FUNCTION(i2cThread, arg) {

  (void)arg;
  chRegSetThreadName("i2c");

  msg_t msg = MSG_OK;
  uint8_t data_to_write = 0;
  i2cflags_t errors;
  int readCount=0;
  uint32_t total = 0;
  uint32_t errCount = 0;
 // systime_t start = chVTGetSystemTime();
  systime_t systick = chVTGetSystemTime();
  uint32_t waitTick = 10;
  while (true) {
    readCount = 2;
    switch(readCount){
      case(0):    //mpu6050 reading
        data_to_write = ADDRESS_ACCEL_XOUT_H;
        i2cAcquireBus(&I2CD1);
        msg = i2cMasterTransmitTimeout(&I2CD1, MPU6050_ADDR, &data_to_write, 1, Buff, 8, 2);
        i2cReleaseBus(&I2CD1);
        break;
      case(1):    //mpu6050 reading
        data_to_write = GYRO_XOUT_H_REG;
        i2cAcquireBus(&I2CD1);
        msg = i2cMasterTransmitTimeout(&I2CD1, MPU6050_ADDR, &data_to_write, 1, Buff, 6, 2);
        i2cReleaseBus(&I2CD1);
        break;
      case(2):    //as5600 reading
        data_to_write = AS5600_REGISTER_ANGLE_HIGH;
        i2cAcquireBus(&I2CD1);
        msg = i2cMasterTransmitTimeout(&I2CD1,AS5600_SLAVE_ADDRESS, &data_to_write, 1, Buff, 2, 2);
        i2cReleaseBus(&I2CD1);
        break;
    }

    total += 1;
    if (msg != MSG_OK) {
      errCount +=1;
       errors = i2cGetErrors(&I2CD1);
       if(errors != 0){
         chprintf((BaseSequentialStream*)&SD3, "errors from I2C: %d / %d code:%x\r\n", errCount,total,errors);
       }
       i2cStop(&I2CD1);
       i2cStart(&I2CD1, &i2cfg1);
    }
    else{

      switch(readCount){
        case(0):    //mpu6050 acc reading
          mpu6050.acx_raw = (Buff[0] << 8) | Buff[1];
          mpu6050.acy_raw= (Buff[2] << 8) | Buff[3];
          mpu6050.acz_raw = (Buff[4] << 8) | Buff[5];
          mpu6050.temp_raw = (Buff[6] << 8) | Buff[7];

          readCount +=1;
          break;
        case(1):    //mpu6050 gyro reading
          mpu6050.gyx_raw = (Buff[0] << 8) | Buff[1];
          mpu6050.gyy_raw = (Buff[2] << 8) | Buff[3];
          mpu6050.gyz_raw = (Buff[4] << 8) | Buff[5];
          readCount +=1;
          break;
        case(2):    //as5600 reading
          mag16 = (Buff[0] << 8) | Buff[1];
          readCount +=1;
          break;
      }

    }

    if(readCount == 3){
      readCount = 0;
      waitTick = 10 - (chVTGetSystemTime() - systick);
      chThdSleepMilliseconds(waitTick);
      //chprintf((BaseSequentialStream*)&SD3, "tick: %d\r\n", chVTGetSystemTime() - start);
     // start = chVTGetSystemTime();
      systick = chVTGetSystemTime();
    }

  }//while

}//static THD_FUNCTION(i2cThread, arg)

void i2c1Init(void){
    palSetPadMode(GPIOB, GPIOB_ARD_D10, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);//GPIOB_PIN6
    palSetPadMode(GPIOB, GPIOB_PIN7, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);

    i2cStart(&I2CD1, &i2cfg1);
    palSetPadMode(GPIOC, GPIOC_ARD_A5, PAL_MODE_OUTPUT_PUSHPULL);//
    int size = sizeof(i2cwaThread);
    chThdCreateStatic(i2cwaThread, sizeof(i2cwaThread), NORMALPRIO, i2cThread, NULL);
    as5600Init();
    mpu6050init();
    //chThdSleepMilliseconds(100);
}

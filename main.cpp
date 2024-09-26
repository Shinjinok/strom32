/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/


#include "ch.h"
#include "hal.h"
//#include "rt_test_root.h"
//#include "oslib_test_root.h"
#include "chprintf.h"
#include "pwm.h"
//#include "shell.h"
#include "usbcfg.h"
#include "mpu6050.h"
#include "as5600.h"
#include "i2cDrv.h"
#include "foc_utils.h"
#include "pid.h"
#include "math.h"


PIDController pid1(1,2,3,4,5);

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)
#define RAD_TO_DEG 57.2957786


/*
 * Red LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;

  chRegSetThreadName("blinker");
  while (true) {
    systime_t time = serusbcfg.usbp->state == USB_ACTIVE ? 100 : 500;
        palClearPad(GPIOA, GPIOA_LED_GREEN);
        chThdSleepMilliseconds(time);
        palSetPad(GPIOA, GPIOA_LED_GREEN);
        chThdSleepMilliseconds(time);
  }
}


void LEDinit(void){

  palSetPadMode(GPIOA, GPIOA_LED_GREEN, PAL_MODE_OUTPUT_PUSHPULL);//
  //Creates the blinker thread.
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
}
/*
 * Application entry point.
 *
*/

void uart3init(void){
  AFIO->MAPR|= AFIO_MAPR_USART3_REMAP_0;// USART3_REMAP[1:0] =  “01” (partial remap) PB10,PB11 -> PC10,PC11
  palSetPadMode(GPIOC, GPIOC_PIN10, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
  palSetPadMode(GPIOC, GPIOC_PIN11, PAL_MODE_INPUT_PULLDOWN);
  sdStart(&SD3, NULL);
}

//#define BUF_SIZE 30
//uint8_t usb_buf[BUF_SIZE];
//uint32_t count;

void USBinit(void){
  /*
     * Initializes a serial-over-USB CDC driver.
     */
    sduObjectInit(&SDU1);
    sduStart(&SDU1, &serusbcfg);

    /*
     * Activates the USB driver and then the USB bus pull-up on D+.
     * Note, a delay is inserted in order to not have to disconnect the cable
     * after a reset.
     */
    usbDisconnectBus(serusbcfg.usbp);
    chThdSleepMilliseconds(1500);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);

    palSetPadMode(GPIOB, GPIOB_PIN5, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPad(GPIOB, GPIOB_PIN5); //enable usb
}
void find_drirection(int motor_number){
  float ang=0.0;
  for(int i = 0;i < 628; i++){
    ang += 0.01;
    setPhaseVoltage(0.1,0,ang);
    chThdSleepMilliseconds(5);
  }


}
int main(void) {
  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */

    halInit();
    chSysInit();
    USBinit();
    LEDinit();
    uart3init();
    pwm_init();
    i2c1Init();

  //shellInit();

  palSetPadMode(GPIOC, GPIOC_BUTTON, PAL_MODE_INPUT_PULLUP);//button gpio setting


  float rangle = 0.0f;
  //float fmag;
  float add = 0.0f;
  while (true) {
    //find_drirection(1);
    float erad = ((float) mag16 / 4096.0 - 0.5) * 2.0*_PI * 7.0;
    float eang = fmod(-erad, 2.0*_PI);
    setPhaseVoltage(0.1,0,eang);
    chprintf((BaseSequentialStream*)&SD3, "mag: %f\r\n", erad);
    chThdSleepMilliseconds(10);

 }
}

//   if (!palReadPad(GPIOC, GPIOC_BUTTON)) {
//      add += 1.0f;
    /*
     if (SDU1.config->usbp->state == USB_ACTIVE) {
       chprintf((BaseSequentialStream*)&SDU1, "mpu: %3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f\r\n",
              mpu6050.ac_x_g,mpu6050.ac_y_g,mpu6050.ac_z_g, mpu6050.gy_x_rps, mpu6050.gy_y_rps, mpu6050.gy_z_rps, mpu6050.temp);
       chprintf((BaseSequentialStream*)&SDU1, "u: %d %d %d %3.1f\r\n", Ua, Ub, Uc);
       chprintf((BaseSequentialStream*)&SDU1, "mag: %3.1f\r\n", mag);
     }*/
//   }
   //rangle = (float) mag16 * as5600_deg_factor * 7.0f - 170.f;
/*    rangle += 1.0f;
  if(rangle < 0.0f){
    rangle += 360.0f;
  }
   while(rangle > 360.0f){
     rangle -= 360.0f;
   }
*/
//   rotation_angle(120.0);
  /*chprintf((BaseSequentialStream*)&SD3, "u: %3.3f %3.3f %3.3f %3.3f %3.3f %3.3f\r\n",
       mpu6050.acx_nomal, mpu6050.acy_nomal, mpu6050.acz_nomal,
       mpu6050.gyx_rps, mpu6050.gyy_rps, mpu6050.gyz_rps);*/
//   chprintf((BaseSequentialStream*)&SD3, "mag: %d\r\n", mag16);

//   chThdSleepMilliseconds(10);

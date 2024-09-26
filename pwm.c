/*
 * pwm.c
 *
 *  Created on: May 2, 2024
 *      Author: sjo
 */

#include "ch.h"
#include "hal.h"
//#include "rt_test_root.h"
//#include "oslib_test_root.h"
#include "chprintf.h"
#include "pwm.h"
#include <math.h>
#include <stdlib.h>
#include "foc_utils.h"

#define D2R 0.0174533f

/*
 * PWM callback.
 * Each time calculate the next duty cycle.
 */
uint16_t Pa = 0;
uint16_t Pb = 0;
 uint16_t Pc = 0;


void setPwm(unsigned int motor,unsigned int a, unsigned int b,unsigned int c);


/*static void pwmcb(PWMDriver *pwmp) {
    (void)pwmp;
    bool downCounting = (PWMD3.tim -> CR1 & 0b10000) >> 4;
    if(downCounting){
      palClearPad(GPIOC, GPIOC_PIN3);
    }
    else {
      palSetPad(GPIOC, GPIOC_PIN3);
    }
}*/

static PWMConfig pwmcfg2 = {
    PWM_FRQ,/* 20MHz PWM clock frequency.     */
    PWM_PERIOD,    /* Initial PWM period 20KHz.         */
   NULL,     /* Period callback.               */
  {
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL}
  },
  0, //CR2
  0, //BDTR
  0 //DIER
};
static PWMConfig pwmcfg3 = { PWM_FRQ, PWM_PERIOD, NULL,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL}
  },
  0, //CR2
  0, //BDTR
  0 //DIER
};
static PWMConfig pwmcfg4 = { PWM_FRQ, PWM_PERIOD, NULL,
  {
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL}
  },
  0, //CR2
  0, //BDTR
  0 //DIER
};

void pwm_init(void){

  //AFIO->MAPR|= AFIO_MAPR_TIM2_REMAP_PARTIALREMAP2; 10: Partial remap (CH1/ETR/PA0, CH2/PA1, CH3/PB10, CH4/PB11)

  palSetPadMode(GPIOB, GPIOB_PIN1, PAL_MODE_STM32_ALTERNATE_PUSHPULL);//A0  TIM3 CH4
  palSetPadMode(GPIOB, GPIOB_ARD_A3, PAL_MODE_STM32_ALTERNATE_PUSHPULL);//B0  TIM3 CH3 GPIOB_PIN0
  palSetPadMode(GPIOA, GPIOA_ARD_D11, PAL_MODE_STM32_ALTERNATE_PUSHPULL);//C0  TIM3 CH2 GPIOA_PIN7

  palSetPadMode(GPIOA, GPIOA_ARD_D12, PAL_MODE_STM32_ALTERNATE_PUSHPULL);//A1  TIM3 CH1 GPIOA_PIN6
  palSetPadMode(GPIOA, GPIOA_ARD_D0, PAL_MODE_STM32_ALTERNATE_PUSHPULL);//B1  TIM2 CH4 GPIOA_PIN3
  palSetPadMode(GPIOA, GPIOA_ARD_D1, PAL_MODE_STM32_ALTERNATE_PUSHPULL);//C1  TIM2 CH3 GPIOA_PIN2

  palSetPadMode(GPIOB, GPIOB_ARD_D14, PAL_MODE_STM32_ALTERNATE_PUSHPULL);//A2  TIM4 CH4 GPIOB_PIN9
  palSetPadMode(GPIOA, GPIOA_ARD_A1, PAL_MODE_STM32_ALTERNATE_PUSHPULL);//B2  TIM2 CH2 GPIOA_PIN1
  palSetPadMode(GPIOB, GPIOB_ARD_D15, PAL_MODE_STM32_ALTERNATE_PUSHPULL);//C3  TIM4 CH3 GPIOB_PIN8


  // It configures PWM related PIN.
  pwmStart(&PWMD2, &pwmcfg2);
  pwmStart(&PWMD3, &pwmcfg3);
  pwmStart(&PWMD4, &pwmcfg4);

  PWMD2.tim -> CR1 = TIM_CR1_CMS_0 ;//center aligned mode
  PWMD4.tim -> CR1 = TIM_CR1_CMS_0;
  PWMD3.tim -> CR1 = TIM_CR1_CMS_0;

  TIM2 -> CNT =0;
  TIM3 -> CNT =0;
  TIM4 -> CNT =0;

  PWMD3.tim -> CR2 = TIM_CR2_MMS_0;//master mode enable
  //PWMD3.tim -> CR2 = TIM_CR2_MMS_2 | TIM_CR2_MMS_1;//OC3REF signal is used as trigger output (TRGO)
  //TIMx slave mode control register (TIMx_SMCR)
  //TS: Trigger selection  010: Internal Trigger 2 (ITR2) |
  //SMS: slave mode selection 110: Trigger mode - The counter starts at a rising edge of the trigger TRGI (but it is not reset).
  PWMD2.tim -> SMCR = TIM_SMCR_TS_1 | ( TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1 );
  PWMD4.tim -> SMCR = TIM_SMCR_TS_1 | ( TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1 );


  PWMD3.tim -> CR1 |= TIM_CR1_CEN;//start timer3


  palSetPadMode(GPIOC, GPIOC_PIN3, PAL_MODE_OUTPUT_PUSHPULL);//
  //pwmEnablePeriodicNotification(&PWMD3);
 // chThdCreateStatic(PWMwaThread, sizeof(PWMwaThread), HIGHPRIO, pwmThread, NULL);


}
void setPwm(unsigned int motor,unsigned int a, unsigned int b,unsigned int c){
  switch (motor){
    case 1:
      pwmEnableChannel(&PWMD3, 3, a);
      pwmEnableChannel(&PWMD3, 2, b);
      pwmEnableChannel(&PWMD3, 1, c);

      break;
    case 2:
      pwmEnableChannel(&PWMD3, 0, a);
      pwmEnableChannel(&PWMD2, 3, b);
      pwmEnableChannel(&PWMD2, 2, c);
      break;
    case 3:
      pwmEnableChannel(&PWMD4, 3, a);
      pwmEnableChannel(&PWMD2, 1, b);
      pwmEnableChannel(&PWMD4, 2, c);
      break;
  }


}
void setPhaseVoltage(float Uq, float Ud, float angle_el){

    float center,Ualpha,Ubeta;
    int sector;
    float _ca,_sa;
    float Ua,Ub,Uc;
      // Sinusoidal PWM modulation
      // Inverse Park + Clarke transformation
      _sincos(angle_el, &_sa, &_ca);

      // Inverse park transform
      Ualpha =  _ca * Ud - _sa * Uq;  // -sin(angle) * Uq;
      Ubeta =  _sa * Ud + _ca * Uq;    //  cos(angle) * Uq;

      // Clarke transform
      Ua = Ualpha;
      Ub = -0.5f * Ualpha + _SQRT3_2 * Ubeta;
      Uc = -0.5f * Ualpha - _SQRT3_2 * Ubeta;

      center = 0.5;
      float Umin = min(Ua, min(Ub, Uc));
      float Umax = max(Ua, max(Ub, Uc));
      center -= (Umax+Umin) / 2;
      Ua += center;
      Ub += center;
      Uc += center;
      Pa = Ua*PWM_PERIOD;
      Pb = Ub*PWM_PERIOD;
      Pc = Uc*PWM_PERIOD;

      setPwm(1, Pa, Pb, Pc);
//      setPwm(2,Ua, Ub, Uc);
//      setPwm(3,Ua, Ub, Uc);

}
/*
void rotation_angle(float Uq){

  // Nice video explaining the SpaceVectorModulation (SVPWM) algorithm
    // https://www.youtube.com/watch?v=QMSWUMEAejg

    float angle_el = deg * D2R;
    // find the sector we are in currently
    int sector = (int) (angle_el * _3_PI) + 1;
    // calculate the duty cycles
    float T1 = _SQRT3* sinf((float) sector*_PI_3 - angle_el) * 0.0f;//Uq/voltage_power_supply;
    float T2 = _SQRT3* sinf(angle_el - (float) (sector-1.0)*_PI_3) * 0.0f;//Uq/voltage_power_supply;
    // two versions possible
    // centered around voltage_power_supply/2
    float T0 = _SQRT3 - T1 - T2;
    // pulled to 0 - better for low power supply voltage

    // calculate the duty cycles(times)
    float Ta,Tb,Tc;

    switch(sector){
    case 1:
        Ta = T1 + T2 + T0/2;
        Tb = T2 + T0/2;
        Tc = T0/2;
        break;
    case 2:
        Ta = T1 +  T0/2;
        Tb = T1 + T2 + T0/2;
        Tc = T0/2;
        break;
    case 3:
        Ta = T0/2;
        Tb = T1 + T2 + T0/2;
        Tc = T2 + T0/2;
        break;
    case 4:
        Ta = T0/2;
        Tb = T1+ T0/2;
        Tc = T1 + T2 + T0/2;
        break;
    case 5:
        Ta = T2 + T0/2;
        Tb = T0/2;
        Tc = T1 + T2 + T0/2;
        break;
    case 6:
        Ta = T1 + T2 + T0/2;
        Tb = T0/2;
        Tc = T1 + T0/2;
        break;
    default:
        // possible error state
        Ta = 0;
        Tb = 0;
        Tc = 0;
    }

    // calculate the phase voltages
    //chMtxLock(&my_mutex);
    Ua = Ta*12;
    Ub = Tb*12;
    Uc = Tc*12;
    //chMtxUnlock(&my_mutex);
    setPwm(1,Ua, Ub, Uc);
    setPwm(2,Ua, Ub, Uc);
    setPwm(3,Ua, Ub, Uc);

    // set the voltages in hardware


}
*/

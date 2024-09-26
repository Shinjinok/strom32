/*
 * pwm.h
 *
 *  Created on: May 2, 2024
 *      Author: sjo
 *
 *
 *
 *      chconf.h
 *      CH_CFG_ST_TIMEDELTA    0 - Time delta constant for the tick-less mode
 */
#ifndef PWM_H_
#define PWM_H_


#include "ch.h"
#include "hal.h"

#define _2_PI 0.6366197723675814
#define _3_PI 0.954929658551372
#define PWM_FRQ 36000000
#define PWM_PERIOD 1000
#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

#ifdef __cplusplus
extern "C"{
#endif

void pwm_init(void);
void rotation_angle(float deg);

void setPwm(unsigned int motor,unsigned int a, unsigned int b,unsigned int c);
void setPhaseVoltage(float Uq, float Ud, float angle_el);

#ifdef __cplusplus
}
#endif
#endif /* PWM_H_ */


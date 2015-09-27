/*
 * sn3218a_led.h
 *
 *  Created on: 16 Oct 2013
 *      Author: kyle zhou
 *
 *  This module is based on a hardware where up to 4 LEDs are connected to sn3218a.
 *  Each LED must use consecutive 3 channels.
 *  The following functions are implemented
 *    - on/off toggle for each led
 *    - activate foreground color for each led
 *    - activate background color for each led
 *    - read foreground/background color status for each led
 *    - foreground/background color toggle for each led
 *    - set r,g,b color for one or more LEDs
 */

#ifndef SN3218A_LED_H_
#define SN3218A_LED_H_

#include "sn3218a.h"
#include <stdbool.h>

#define SN3218A_MAX_LEDS       4 // max number of leds

/* LED On/Off Toggle
 * The color is not changed
 * It does not take effects until sn3218a_data_update() is called
 * led: 0 - SN3218A_MAX_LEDS-1
 */
void sn3218a_led_toggle( int led );


/* LED takes on foreground color (if it is on)
 * It does not take effects until sn3218a_data_update() is called
 */
void sn3218a_led_take_fg( int led );

/* LED takes on background color (if it is on)
 * It does not take effects until sn3218a_data_update() is called
 */
void sn3218a_led_take_bg( int led );

/* Get LED fg/bg state - 1 fg; 0 bg */
bool sn3218a_led_state_fb( int led );

/* LED Toggle between fg & bg colors (if it is on)
 * It does not take effects until sn3218a_data_update() is called
 */
void sn3218a_led_toggle_fb( int led );


/* initalize the SN3218A based LEDs
 * idx: index of i2c bus
 * fb: operating mode. 1 - fg/bg mode; 0 - fg only mode
 */
int sn3218a_led_init( int idx, bool fb );

void sn3218a_led_uninit( void );

/* Set LEDs' color
 * leds: bitmap of LEDs
 *        fg: 0x01 - LED 1; 0x02 - LED 2; 0x04 - LED 3; 0x08 - LED 4
 *        bg: 0x10 - LED 1; 0x20 - LED 2; 0x40 - LED 3; 0x80 - LED 4
 *       leds can be bitor'd to set multiple LEDs/colors at a time.
 * NOTE: this function sets the shadow variable sn3218a_led_color[]
 *       and write to PWM registers if the changes are in effect
 */
void sn3218a_led_set_color( int leds, uint8_t r, uint8_t g, uint8_t b);

#endif /* SN3218A_LED_H_ */

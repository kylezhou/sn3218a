/*
 * sn3218a.h
 *
 *  Created on: 16 Oct 2013
 *      Author: kyle zhou
 *
 *  sn3218a is a 18-channel LED driver IC. Each channel is a 256-level PWM generator.
 *  Its interface is I2C with slave address 0xa8. It only supports writing but not reading.
 *
 *  This module encapsulates the following functionalities
 *   - i2c initialization,
 *   - setting pwm level for each channel,
 *   - turning on/off each channel,
 *   - turning on/off all channels at once,
 *   - reading channel on/off status.
 *  It also implements basic led manipulation with each led using 3 channels
 *   - reading led on/off status,
 *   - turning on/off each led,
 *   - setting color for each led.
 *  Note: the above led manipulation functions depend on led to channel mapping, which is hardware dependent.
 *        the user of this module shall provide the mapping.
 */

#ifndef SN3218A_H_
#define SN3218A_H_

#include <stdbool.h>
#include <stdint.h>

#define SN3218A_SLAVE_ADDR (0xa8 >> 1)

// software shutdown register
#define	SN3218A_REG_CHIP_SSD     0x00
#define SN3218A_SSD_SHUTDOWN     0x00 // shutdown mode
#define SN3218A_SSD_OPERATING    0x01 // operating mode

// PWM registers 1-18
#define	SN3218A_REG_CTRL1_D1     0x01
#define	SN3218A_REG_CTRL1_D2     0x02
#define	SN3218A_REG_CTRL1_D3     0x03
#define	SN3218A_REG_CTRL1_D4     0x04
#define	SN3218A_REG_CTRL1_D5     0x05
#define	SN3218A_REG_CTRL1_D6     0x06

#define	SN3218A_REG_CTRL2_D7     0x07
#define	SN3218A_REG_CTRL2_D8     0x08
#define	SN3218A_REG_CTRL2_D9     0x09
#define	SN3218A_REG_CTRL2_D10    0x0a
#define	SN3218A_REG_CTRL2_D11    0x0b
#define	SN3218A_REG_CTRL2_D12    0x0c

#define	SN3218A_REG_CTRL3_D13    0x0d
#define	SN3218A_REG_CTRL3_D14    0x0e
#define	SN3218A_REG_CTRL3_D15    0x0f
#define	SN3218A_REG_CTRL3_D16    0x10
#define	SN3218A_REG_CTRL3_D17    0x11
#define	SN3218A_REG_CTRL3_D18    0x12

// control registers
#define	SN3218A_REG_CTRL1        0x13 // channel 1-6
#define	SN3218A_REG_CTRL2        0x14 // channel 7-12
#define	SN3218A_REG_CTRL3        0x15 // channel 13-18

// data update register. data written to PWM & control registers only take effects after the data update register is written with any data.
#define	SN3218A_REG_DATA_UPDATE  0x16

// reset register
#define	SN3218A_REG_SW_RST       0x17

int sn3218a_init( int idx );
void sn3218a_uninit( void );
void sn3218a_data_update( void );
void sn3218a_set_brightness( int ch, uint8_t brightness );
void sn3218a_set_brightness_triplet(int ch, uint8_t brightness1, uint8_t brightness2, uint8_t brightness3 );
void sn3218a_all_on( void );
void sn3218a_all_off( void );
void sn3218a_turn_on( int ch );
void sn3218a_turn_off( int ch );
bool sn3218a_get_state( int ch );

bool sn3218a_led_get_state( int led );
void sn3218a_led_set_rgb_color( int led, uint8_t r, uint8_t g, uint8_t b );
void sn3218a_led_turn_on( int led );
void sn3218a_led_turn_off( int led );

#endif /* SN3218A_H_ */

/*
 * sn3218a_led_def.c
 *
 *  Created on: 16 Nov 2013
 *      Author: kyle zhou
 *
 *  This file defines the mapping of LEDs to the sn3218a channels
 */
#include <stdint.h>

/* LED RGB to SN3218A channel mapping - hardware dependent code
 * LED 1: (r, g, b) - ch ( 3,  2,  1) - ctrl1 [2,1,0]
 * LED 2: (r, g, b) - ch (18, 17, 16) - ctrl3 [5,4,3]
 * LED 3: (r, g, b) - ch (15, 14, 13) - ctrl3 [2,1,0]
 * LED 4: (r, g, b) - ch ( 6,  5,  4) - ctrl1 [5,4,3]
 */
int sn3218a_led_red_ch[] = {2, 17, 14, 5}; // 0-based index
int sn3218a_led_green_ch[] = {1, 16, 13, 4};
int sn3218a_led_blue_ch[] = {0, 15, 12, 3};

int sn3218a_led_ctrl_idx[] = {0, 2, 2, 0};
uint8_t sn3218a_led_mask[] = {0x07, 0x38, 0x07, 0x38};

// end - hardware dependent code

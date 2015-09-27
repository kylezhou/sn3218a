/*
 * sn3218a_led.c
 *
 *  Created on: 16 Oct 2013
 *      Author: kyle zhou
 */
#include <stdint.h>
#include <stdio.h>
#include "sn3218a_led.h"

/* local variables */

/* foreground & background colors for LEDs
 * stored as R,G,B triplets, all fg colors followed by all bg colors
 * default colors: fg - green, red, blue, yellow; bg - all grey
 */
static uint8_t sn3218a_led_color[SN3218A_MAX_LEDS*3*2] = { \
    0x00,0xff,0x00, 0xff,0x00,0x00, 0x00,0x00,0xff, 0xff,0xff,0x00, \
    0x50,0x50,0x50, 0x50,0x50,0x50, 0x50,0x50,0x50, 0x50,0x50,0x50 };

/* fg/bg state for LEDs, which indicates if fg or bg should be used when LED is on
 * combining led_state with led_color determines actual PWM register values
 */
static bool sn3218a_led_state[SN3218A_MAX_LEDS];

/* initalize the SN3218A based LEDs
 * idx: i2c index
 * fb: operating mode. 1 - fg/bg mode; 0 - fg only mode
 * return: 0 for success
 */
int sn3218a_led_init( int idx, bool fb )
{
	int i;
	if(sn3218a_init(idx))
	{
		fprintf(stderr, "Failed to initialize sn3218a LEDs!\n");
		return 1;
	}
	for(i=0; i<SN3218A_MAX_LEDS; i++)
	{
		if(fb)
		{
			sn3218a_led_take_bg(i);
			sn3218a_led_turn_on(i);
		}
		else
		{
			sn3218a_led_turn_off(i);
			sn3218a_led_take_fg(i);
		}
	}
	sn3218a_data_update();
	return 0;
}

void sn3218a_led_uninit( void )
{
	sn3218a_uninit();
}

void sn3218a_led_toggle( int led )
{
	if(sn3218a_led_get_state(led))
		sn3218a_led_turn_off(led);
	else
		sn3218a_led_turn_on(led);
}

/* LED takes on foreground color (if it is on)
 * It does not take effects until sn3218a_data_update() is called
 */
void sn3218a_led_take_fg( int led )
{
	sn3218a_led_set_rgb_color(led, sn3218a_led_color[led*3], sn3218a_led_color[led*3+1], sn3218a_led_color[led*3+2]);
	sn3218a_led_state[led] = 1;
}

/* LED takes on background color (if it is on)
 * It does not take effects until sn3218a_data_update() is called
 */
void sn3218a_led_take_bg( int led )
{
	sn3218a_led_set_rgb_color(led, sn3218a_led_color[SN3218A_MAX_LEDS*3 + led*3], sn3218a_led_color[SN3218A_MAX_LEDS*3 + led*3+1], sn3218a_led_color[SN3218A_MAX_LEDS*3 + led*3+2]);
	sn3218a_led_state[led] = 0;
}

/* Get LED fg/bg state - 1 fg; 0 bg */
bool sn3218a_led_state_fb( int led )
{
	return sn3218a_led_state[led];
}

/* LED Toggle between fg & bg colors (if it is on)
 * It does not take effects until sn3218a_data_update() is called
 */
void sn3218a_led_toggle_fb( int led )
{
	if(sn3218a_led_state_fb(led))
		sn3218a_led_take_bg(led);
	else
		sn3218a_led_take_fg(led);
}


/* Set LEDs' color
 * leds: bitmap of LEDs
 *        fg: 0x01 - LED 1; 0x02 - LED 2; 0x04 - LED 3; 0x08 - LED 4
 *        bg: 0x10 - LED 1; 0x20 - LED 2; 0x40 - LED 3; 0x80 - LED 4
 *       leds can be bitor'd to set multiple LEDs/colors at a time.
 * NOTE: this function sets the shadow variable sn3218a_led_color[]
 *       and write to PWM registers if the changes are in effect
 */
void sn3218a_led_set_color( int leds, uint8_t r, uint8_t g, uint8_t b)
{
  int i;
  for(i = 0; leds && i < SN3218A_MAX_LEDS * 2; i++)
  {
    if( leds & 1 )
    {
      sn3218a_led_color[i*3] = r;
      sn3218a_led_color[i*3+1] = g;
      sn3218a_led_color[i*3+2] = b;
      if(i<SN3218A_MAX_LEDS)
      {
    	  if(sn3218a_led_state_fb(i))
    		  sn3218a_led_take_fg(i);
      }
      else
      {
    	  if(!sn3218a_led_state_fb(i-SN3218A_MAX_LEDS))
    		  sn3218a_led_take_bg(i-SN3218A_MAX_LEDS);
      }
    }
    leds >>= 1;
  }
}

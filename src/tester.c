/*
 * tester.c
 *
 *  Created on: 16 Oct 2013
 *      Author: kyle zhou
 *  This is a tester program for sn3218a_led.
 *  The hardware has 4 LEDs attached to 12 channels of sn3218a.
 *  The mappings are defined in sn3218a_led_def.c.
 *  After initializing i2c bus and LEDs, the indefinite loop will set
 *  four colors, red, green, blue, yellow, to the four LEDs in a clockwise turn,
 *  and move forward cyclicly per second.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sn3218a_led.h"

int main( int argc, char *argv[] )
{
	int idx = 2;
	int led;
	if(argc > 1)
	{
		idx = atoi(argv[1]);
	}
	if(sn3218a_led_init(idx, 0))
	{
		fprintf(stderr, "Failed to initialize!\n");
		return 1;
	}

	for(led=0; led<4; led++)
		sn3218a_led_turn_on(led);

	led = 0;
	while(1)
	{
		sn3218a_led_set_color(1<<led, 255, 0, 0);
		sn3218a_led_set_color(1<<((led+1)%4), 0, 255, 0);
		sn3218a_led_set_color(1<<((led+2)%4), 0, 0, 255);
		sn3218a_led_set_color(1<<((led+3)%4), 255, 255, 0);

		sn3218a_data_update();
		led++;
		if(led>3)
			led =0;
		sleep(1);
	}

	sn3218a_led_uninit();
	return 0;
}

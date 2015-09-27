/*
 * sn3218a.c
 *
 *  Created on: 16 Oct 2013
 *      Author: kyle zhou
 */
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "sn3218a.h"

#define MAX_DEVNAME_LEN 20

/* local variables */
static int file;
static uint8_t sn3218a_ctrl[3] = {0, 0, 0};

/* local functions */
static int sn3218a_write_reg( uint8_t reg, uint8_t data[], int len );
static int sn3218a_write_reg_single( uint8_t reg, uint8_t data );
static int sn3218a_write_reg_double( uint8_t reg, uint8_t data1, uint8_t data2 );
static int sn3218a_write_reg_triple( uint8_t reg, uint8_t data1, uint8_t data2, uint8_t data3 );

/* external variables - these are hardware dependent */
extern int sn3218a_led_red_ch[];
extern int sn3218a_led_green_ch[];
extern int sn3218a_led_blue_ch[];
extern int sn3218a_led_ctrl_idx[];
extern uint8_t sn3218a_led_mask[];

int sn3218a_init(int idx)
{
	char devname[MAX_DEVNAME_LEN];
	snprintf(devname, MAX_DEVNAME_LEN, "/dev/i2c-%d", idx);
	if((file=open(devname, O_WRONLY)) < 0)
	{
		fprintf(stderr, "Failed to open %s! errno=%d [%s]\n", devname, errno, strerror(errno));
		return 1;
	}
	if(ioctl(file, I2C_SLAVE, SN3218A_SLAVE_ADDR) < 0)
	{
		fprintf(stderr, "I2C_SLAVE addr %02x failed! errno=%d [%s]\n", SN3218A_SLAVE_ADDR, errno, strerror(errno));
		close(file);
		return 2;
	}
	/* set operating mode*/
	if(sn3218a_write_reg_single( SN3218A_REG_CHIP_SSD, SN3218A_SSD_OPERATING ))
	{
		fprintf(stderr, "Failed to set operating mode!\n");
		close(file);
		return 3;
	}

	return 0;
}

void sn3218a_uninit( void )
{
	if(file)
	{
		close(file);
		file = 0;
	}
}

static int sn3218a_write_reg( uint8_t reg, uint8_t data[], int len )
{
	uint8_t buf[len+1];
	buf[0] = reg;
	memcpy(buf+1, data, len);
	if(write(file, buf, len+1) != len+1)
	{
		fprintf(stderr, "Failed to write to register %x! errno=%d [%s]\n", reg, errno, strerror(errno));
		return 1;
	}
	return 0;
}

static int sn3218a_write_reg_single( uint8_t reg, uint8_t data)
{
	uint8_t buf[2];
	buf[0] = reg;
	buf[1] = data;
	if(write(file, buf, 2) != 2)
	{
		fprintf(stderr, "Failed to write to register %x! errno=%d [%s]\n", reg, errno, strerror(errno));
		return 1;
	}
	return 0;
}

static int sn3218a_write_reg_double( uint8_t reg, uint8_t data1, uint8_t data2)
{
	uint8_t buf[3];
	buf[0] = reg;
	buf[1] = data1;
	buf[2] = data2;
	if(write(file, buf, 3) != 3)
	{
		fprintf(stderr, "Failed to write to register %x! errno=%d [%s]\n", reg, errno, strerror(errno));
		return 1;
	}
	return 0;
}

static int sn3218a_write_reg_triple( uint8_t reg, uint8_t data1, uint8_t data2, uint8_t data3)
{
	uint8_t buf[4];
	buf[0] = reg;
	buf[1] = data1;
	buf[2] = data2;
	buf[3] = data3;
	if(write(file, buf, 4) != 4)
	{
		fprintf(stderr, "Failed to write to register %x! errno=%d [%s]\n", reg, errno, strerror(errno));
		return 1;
	}
	return 0;
}

void sn3218a_data_update( void )
{
	if(sn3218a_write_reg_single(SN3218A_REG_DATA_UPDATE, 0))
	{
		fprintf(stderr, "Failed to update!\n");
	}
}

void sn3218a_set_brightness( int ch, uint8_t brightness )
{
	sn3218a_write_reg_single(SN3218A_REG_CTRL1_D1+ch, brightness);
}

void sn3218a_set_brightness_triplet(int ch, uint8_t brightness1, uint8_t brightness2, uint8_t brightness3 )
{
	sn3218a_write_reg_triple(SN3218A_REG_CTRL1_D1+ch, brightness1, brightness2, brightness3 );
}

void sn3218a_all_on( void )
{
	sn3218a_ctrl[0] = sn3218a_ctrl[1] = sn3218a_ctrl[2] = 0x3f;
	sn3218a_write_reg_single( SN3218A_REG_CHIP_SSD, SN3218A_SSD_OPERATING );
	sn3218a_write_reg( SN3218A_REG_CTRL1, sn3218a_ctrl, 3 );
	sn3218a_data_update();
}

void sn3218a_all_off( void )
{
	sn3218a_ctrl[0] = sn3218a_ctrl[1] = sn3218a_ctrl[2] = 0;
	sn3218a_write_reg_single( SN3218A_REG_CHIP_SSD, SN3218A_SSD_SHUTDOWN );
	sn3218a_write_reg( SN3218A_REG_CTRL1, sn3218a_ctrl, 3 );
	sn3218a_data_update();
}

void sn3218a_turn_on( int ch )
{
	if ( ch >= 0 && ch < 6 )
	{
		sn3218a_ctrl[0] |=  1 << ch;
		sn3218a_write_reg_single(SN3218A_REG_CTRL1, sn3218a_ctrl[0]);
	}
	else if ( ch >= 6 && ch < 12 )
	{
		sn3218a_ctrl[1] |= 1 << (ch-6);
		sn3218a_write_reg_single(SN3218A_REG_CTRL2, sn3218a_ctrl[1]);
	}
	else if ( ch >= 12 && ch < 18 )
	{
		sn3218a_ctrl[2] |= 1 << (ch-12);
		sn3218a_write_reg_single(SN3218A_REG_CTRL3, sn3218a_ctrl[2]);
	}
}

void sn3218a_turn_off( int ch )
{
	if ( ch >= 0 && ch < 6 )
	{
		sn3218a_ctrl[0] &= ~( 1 << ch );
		sn3218a_write_reg_single(SN3218A_REG_CTRL1, sn3218a_ctrl[0]);
	}
	else if ( ch >= 6 && ch < 12 )
	{
		sn3218a_ctrl[1] &= ~( 1 << (ch-6) );
		sn3218a_write_reg_single(SN3218A_REG_CTRL2, sn3218a_ctrl[1]);
	}
	else if ( ch >= 12 && ch < 18 )
	{
		sn3218a_ctrl[2] &= ~( 1 << (ch-12) );
		sn3218a_write_reg_single(SN3218A_REG_CTRL3, sn3218a_ctrl[2]);
	}
}

bool sn3218a_get_state( int ch )
{
	if ( ch >= 0 && ch < 6 )
	{
		return !!( sn3218a_ctrl[0] & ( 1 << ch ) );
	}
	if ( ch >= 6 && ch < 12 )
	{
		return !!( sn3218a_ctrl[1] & ( 1 << (ch-6) ) );
	}
	if ( ch >= 12 && ch < 18 )
	{
		return !!( sn3218a_ctrl[2] & ( 1 << (ch-12) ) );
	}
	return 0;
}

/* the following are LED related codes */

/* RGB color setters.
 */
void sn3218a_led_set_rgb_color( int led, uint8_t r, uint8_t g, uint8_t b )
{
	sn3218a_set_brightness(sn3218a_led_red_ch[led], r);
	sn3218a_set_brightness(sn3218a_led_green_ch[led], g);
	sn3218a_set_brightness(sn3218a_led_blue_ch[led], b);
}

/* LED Turn On
 * The color is not changed
 * It does not take effects until SN3218A_DATA_UPDATE() is called
 */
void sn3218a_led_turn_on( int led )
{
	sn3218a_ctrl[sn3218a_led_ctrl_idx[led]] |= sn3218a_led_mask[led];
	sn3218a_write_reg_single(SN3218A_REG_CTRL1+sn3218a_led_ctrl_idx[led], sn3218a_ctrl[sn3218a_led_ctrl_idx[led]]);
}

/* LED Turn Off
 * The color is not changed
 * It does not take effects until SN3218A_DATA_UPDATE() is called
 */
void sn3218a_led_turn_off( int led )
{
	sn3218a_ctrl[sn3218a_led_ctrl_idx[led]] &= ~sn3218a_led_mask[led];
	sn3218a_write_reg_single(SN3218A_REG_CTRL1+sn3218a_led_ctrl_idx[led], sn3218a_ctrl[sn3218a_led_ctrl_idx[led]]);
}

/* Get LED On/Off State
 * TRUE/On when at least one channel is on
 * FALSE/Off when all 3 channels are off
 */
bool sn3218a_led_get_state( int led )
{
	return !!(sn3218a_ctrl[sn3218a_led_ctrl_idx[led]] & sn3218a_led_mask[led]);
}

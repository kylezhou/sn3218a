#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main( void )
{
	int file;
	char devname[] = "/dev/i2c-2";
	int slave_addr = 0xa8>>1;
	char buf[32];
	int i;

	if((file=open(devname, O_WRONLY)) < 0)
	{
		puts("Failed to open i2c");
		return 1;
	}
	if(ioctl(file, I2C_SLAVE, slave_addr) < 0)
	{
		printf("I2C_SLAVE addr %02x failed! errno=%d [%s]\n", slave_addr, errno, strerror(errno));
		return 2;
	}

	/* write 1 to addr 0 */
	buf[0] = 0;
	buf[1] = 1;
	if(write(file, buf, 2) != 2)
	{
		printf("Failed to enable! errno=%d [%s]\n", errno, strerror(errno));
		return 3;
	}

	/* write to addr 1-18 */
	buf[0] = 1;
	for(i=1; i<0x13; i++)
	{
		buf[i] = 255; // full on
	}
	buf[2] = buf[3] = 0;
	buf[4] = buf[6] = 0;
	buf[13] = buf[14] = 0;
	buf[16] = 0;
	if(write(file, buf, 19) != 19)
	{
		printf("Failed to set pwm! errno=%d [%s]\n", errno, strerror(errno));
		return 4;
	}

	/* write to addr 19-21 */
	buf[0] = 0x13;
	for(i=1; i<4; i++)
	{
		buf[i] = 0b00111111; // all on
	}
	if(write(file, buf, 4) != 4)
	{
		printf("Failed to turn on! errno=%d [%s]\n", errno, strerror(errno));
		return 5;
	}

	/* update */
	buf[0] = 0x16;
	buf[1] = 0;
	if(write(file, buf, 2) != 2)
	{
		printf("Failed to update! errno=%d [%s]\n", errno, strerror(errno));
		return 6;
	}

	return 0;
}

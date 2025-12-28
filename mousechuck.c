#include <u.h>
#include <libc.h>


#define I2CADDR 0x52;




void
main(int argc, char *argv[])
{
	uchar wbuf[2];
	uchar rbuf[6];
	uchar bbuf[16];
	int fd, i;
	int sx, sy, ax, ay, az, bc, bz, but;

	char mbuf[80];
	int mifd, z;
	z = 0;

	memset(bbuf, 0, sizeof(bbuf));

	if(access("/dev/i2c1/i2c.52.data", 0) != 0)
		bind("#J", "/dev", MBEFORE);

	fd = open("/dev/i2c1/i2c.52.data", ORDWR);
	if(fd < 0)
		sysfatal("cant open i2c");

	mifd = open("/dev/mousein", OWRITE);
		if(mifd < 0)
		sysfatal("cant open mouse");

/* turn off encryption */
	wbuf[0] = 0xF0;
	wbuf[1] = 0x55;
	pwrite(fd, wbuf,2, 0);
	sleep(20);

	wbuf[0] = 0xFB;
	wbuf[1] = 0x00;
	pwrite(fd, wbuf, 2, 0);
	sleep(20);

	wbuf[0] = 0xFA;
	pwrite(fd, wbuf, 1, 0);
	sleep(20);
	pread(fd, bbuf, 16, 0);

	for(i = 0; i < 16; i++){
		print("%uX-", bbuf[i]);
	}
	print("\n");

	wbuf[0] = 0x00;
	while(1){
		sleep(20);
		pwrite(fd, wbuf, 1, 0);
		sleep(20);
		pread(fd, rbuf, 6, 0);

		sx = rbuf[0];
		sy = rbuf[1];
		ax = ((rbuf[5] & 0xC0) >> 6) | (rbuf[2] << 2);
		ay = ((rbuf[5] & 0x30) >> 4) | (rbuf[3] << 2);
		az = ((rbuf[5] & 0x0C) >> 2) | (rbuf[4] << 2);
		bc = (rbuf[5] & 0x02) >> 1;
		bz = rbuf[5] & 0x01;

		sx -= 35;
		sy -= 37;
		sx -= 96;
		sy -=96;
		

		sy *= -1;

		bc = (bc == 1) ? 0 : 1;
		bz = (bz == 1) ? 0 : 1;

		print("sx:%d sy:%d ax:%d ay:%d az:%d bc:%d bz:%d\n", sx, sy, ax, ay, az, bc, bz);

		but = 0;

		if(bc == 1)
			but |= 0x01;

		if(bz == 1)
			but |= 0x04;

		if(ay < 400)
			but |= 0x08;

		if(ay > 600)
			but |= 0x10;

		if(!((sy > 1) || (sy < -2)))
			sy = 0;

		if(!((sx > 1) || (sx < -2)))
			sx = 0;



		seprint(mbuf, mbuf + sizeof(mbuf), "m%11d %11d %11d %11d", sx, sy, but, z);

		print("%s\n", mbuf);

		write(mifd, mbuf, strlen(mbuf));
	}


	exits(nil);
}

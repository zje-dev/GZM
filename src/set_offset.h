#include <fstream>
#include <string>

unsigned long byteSwapULong(unsigned long);

unsigned long update_crc (unsigned long, unsigned char*, int);

unsigned long crc (unsigned char*, int);

struct grabChunk { //Randy Heit is the best
	char size[4];
	char name[4];
	unsigned char xoff[4];
	unsigned char yoff[4];
	unsigned long crc;
};

void check_and_write (const char*, const char*, int, int);

#include <fstream>
#include <string>
#include "set_offset.h"

using namespace std;

//code by Randy Heit 

unsigned long byteSwapULong (unsigned long value) {
	return (((value&0x000000FF)<<24)+((value&0x0000FF00)<<8)+((value&0x00FF0000)>>8)+((value&0xFF000000)>>24));
}

unsigned long update_crc (unsigned long crc, unsigned char *buf, int len) {
	unsigned long crc_table[256];
	unsigned long c = crc;
	int k(0), n(0);
	for (n = 0; n < 256; n++) {
		c = (unsigned long) n;
		for (k = 0; k < 8; k++) {
			if (c & 1)
				c = 0xedb88320L ^ (c >> 1);
			else
				c = c >> 1;
		}
		crc_table[n] = c;
	}
	for (n = 0; n < len; n++)
		c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
	return c;
}

unsigned long crc (unsigned char *buf, int len) {
	return update_crc(0xffffffffL, buf, len) ^ 0xffffffffL;
}

void check_and_write (const char *file_name, const char *out_name, int x_off, int y_off) {
	string line, res;
	ifstream file;
	bool has_grAb = false;
	int grabPos = 33;
	file.open(file_name, fstream::in);
	if (file.is_open()) {
		while (getline (file, line)) {
			res += line + "\n";
		}
	}
	file.close();
	for (int i = 0; i < res.length(); i++) {
		if (res[i]=='g' && res[i+1]=='r' && res[i+2]=='A' && res[i+3]=='b') {
			has_grAb = true;
			grabPos = (i - 4);
			break;
		}
	}
	if (!has_grAb) {
		ofstream out_file(out_name,ios::binary|ios::out);
		grabChunk chunk = { 
			{0,0,0,8},
			{'g', 'r', 'A', 'b'}, 
			{
				static_cast<unsigned char>(0),
				static_cast<unsigned char>(0),
				static_cast<unsigned char>(0),
				static_cast<unsigned char>(x_off)
			},
			{
				static_cast<unsigned char>(0),
				static_cast<unsigned char>(0),
				static_cast<unsigned char>(0),
				static_cast<unsigned char>(y_off)
			},
			0
		};
		chunk.crc = byteSwapULong(crc((unsigned char*)&chunk+4, 12));
		out_file.write(res.c_str(), grabPos);
		out_file.write((char *)&chunk, 20);
		int filePos = grabPos + 0;
		out_file.write(res.c_str() + filePos, res.length() - filePos);
	} else {
		ofstream out_file(out_name,ios::binary|ios::out);
		out_file.write(res.c_str(), res.length());
	}
}

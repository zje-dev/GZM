#include <string>
#include <fstream>
#include <iostream>
#include <gtk/gtk.h>

#include "files.h"
#include "leng.h"

/*
	This file is for wad-handle
*/

struct lump_data {
	int position;
	int length;
	std::string name;
};

int get_header_data (int, unsigned char*);
string get_lump_name (int, char*);
void load_wad (string);

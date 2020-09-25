#pragma once
#include <string>
#include <gtk/gtk.h>
#include <fstream>

using namespace std;

/*
	This file is for file-handle
*/

void file_write (const char*, const char*, const char*);
string read_config (const char*);
string read_project (const char*);
string read_file (const char*, const char*);
void write_config (const char*, const char*);
void write_project (const char*, const char*);
void write_frame (const char*, const char*);
void remove_frame (const char*, const char*);
void re_frame (const char*, const char*, string, GtkBuilder*);
int sprites_length ();

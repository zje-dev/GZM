#include <string>
#include <fstream>
#include <gtk/gtk.h>
#include <ostream>
#include "files.h"

using namespace std;

/*
	GZM = config
	TMP = Temporal data
*/

void file_write (const char *fil, const char *key, const char *value) { //updates data of the .cfg (i can change that to .ini)
/*
	FIL = File name
	KEY = Name of the value
	VALUE = The new value
*/
	string line, res = "";
	ifstream file;
	file.open(fil, fstream::in);
	if (file.is_open()) {
		while (getline (file, line)) {
			if (line.substr(0, line.find("=")) == key) {
				res += line.substr(0, line.find("=") + 1) + value + "\n";
			} else {
				res += line + "\n";
			}
		}
	}
	file.close();
	ofstream elif;
	elif.open(fil, fstream::out);
	elif << res;
	elif.close();
}

string read_config (const char* filename, const char* key) { //gets the data of value
/*
	Filename = The name of the file
	KEY = Name of the value
*/
	string line, res;
	ifstream file;
	file.open(filename, fstream::in);
	if (file.is_open()) {
		while (getline (file, line)) {
			if (line.substr(0, line.find("=")) == key)
				res = line.substr(line.find("=") + 1, line.length());
		}
	}
	file.close();
	return res;
}

string read_config (const char* key) {return read_config("./config/gzm.cfg", key);} //reads the value of GZM
string read_project (const char* key) {return read_config("./config/tmp.cfg", key);} //reads the value of TMP

void write_config (const char *key, const char *value) {//replaces the value of GZM
	file_write("./config/gzm.cfg", key, value);
}

void write_project (const char *key, const char *value) {//replaces the value of TMP
	file_write("./config/tmp.cfg", key, value);
}

void write_frame (const char *name, const char *value) {//adds a new frame
/*
	NAME = Name of the state
	VALUE = Frame data
*/
	string line, res = "";
	ifstream file;
	file.open(string("./config/animations/")+name, fstream::in);
	if (file.is_open()) {
		while (getline (file, line)) {
			res += line + "\n";
		}
	}
	file.close();
	ofstream anim;
	anim.open(string("./config/animations/")+name, fstream::out);
	anim << res;
	anim << value;
	anim.close();
}

void remove_frame (const char *name, const char *value) { //removes a frame
/*
	NAME = Name of the state
	VALUE = Frame data
*/
	string line, res = "";
	ifstream file;
	file.open(string("./config/animations/")+name, fstream::in);
	if (file.is_open()) {
		while (getline (file, line)) {
			if (line != value)
				res += line + "\n";
		}
	}
	file.close();
	ofstream anim;
	anim.open(string("./config/animations/")+name, fstream::out);
	anim << res;
	anim.close();
}

void re_frame (const char *name, const char *original_value, string new_value, GtkBuilder *builder) { // reload frames
/*
	NAME = State name
	ORIGINAL_VALUE = The old frame
	NEW_VALUE = The new frame
	BUILDER = Window
*/
	GList *children, *iter;
	children = gtk_container_get_children(GTK_CONTAINER(gtk_builder_get_object(builder, "sprites")));
	for(iter = children; iter != NULL; iter = g_list_next(iter)) {
		if (string(gtk_button_get_label(GTK_BUTTON(iter->data))) 
			== 
		string(gtk_widget_get_name(GTK_WIDGET(gtk_builder_get_object(builder, "rem_sprite")))))
			gtk_button_set_label(GTK_BUTTON(iter->data), new_value.c_str());
	}
	g_list_free(children);
	gtk_widget_set_name(GTK_WIDGET(gtk_builder_get_object(builder, "rem_sprite")), new_value.c_str());
	string line, res = "";
	ifstream file;
	file.open(string("./config/animations/")+name, fstream::in);
	if (file.is_open()) {
		while (getline (file, line)) {
			if (line != original_value)
				res += line + "\n";
			else
				res += new_value + "\n";
		}
	}
	file.close();
	ofstream anim;
	anim.open(string("./config/animations/")+name, fstream::out);
	anim << res;
	anim.close();
}

int sprites_length () { //returns the length of all sprites
	string file = "", line;
	int res = 0;
	for (int i = 0; i < read_project("states.sprites_names").length(); i++) {
		if (read_project("states.sprites_names").at(i) == ';') {
			ifstream sprite;
			sprite.open(
				(read_project("file.path") + "/" + read_project("project.name") + "/sprites/" + file)
			, fstream::in);
			if (sprite.is_open()) {
				sprite.seekg(0, ios::end);
				res += sprite.tellg();
			}
			sprite.close();
			file = "";
		} else {
			file += read_project("states.sprites_names").at(i);
		}
	}
	return res;
}

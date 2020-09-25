#include <string>
#include <fstream>
#include <iostream>
#include <gtk/gtk.h>

#include "files.h"
#include "leng.h"
#include "load_pk3.h"
#include "zscript.h"
#include "decorate.h"
#include "editors.h"

void load_pk3 (string name) {
	system((string("unzip \"")+name+"\" -d \""+name.substr(0, name.length() - 4)+"\"").c_str());
	int folder = 0;
	for (int i = name.length() - 1; i != 0; i--) {
		if (name.at(i) == '/')
			break;
		else
			folder = i;
	}
	write_project("file.path", name.substr(0, folder).c_str());
	write_project("project.name", read_project("project.name").substr(0, read_project("project.name").length() - 4).c_str());
	parse_decorate();
	parse_zscript();
}

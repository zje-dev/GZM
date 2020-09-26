#include <string>
#include <fstream>
#include <iostream>
#include <gtk/gtk.h>
#include <vector>
#include <experimental/filesystem>

#include "files.h"
#include "leng.h"
#include "decorate.h"
#include "zscript.h"
#include "load_wad.h"
#include "editors.h"

int get_header_data (int offset, unsigned char *buffer) {
	return (buffer[offset + 3] << 24 | buffer[offset + 2] << 16 | buffer[offset + 1] << 8 | buffer[offset]);
}

string get_lump_name (int offset, char *buffer) {
	string res = "";
	for (int i = 0; i < 8; i++) {
		if (int(buffer[offset + i]) != 0)
			res += buffer[offset + i];
	}
	return res;
}

void load_wad (string name) {
	guint32 folder = 0,
	lumps = 0,
	header_folder = 0;
	for (int i = name.length() - 1; i != 0; i--) {
		if (name.at(i) == '/')
			break;
		else
			folder = i;
	}
	write_project("file.path", name.substr(0, folder).c_str());
	write_project("project.name", read_project("project.name").substr(0, read_project("project.name").length() - 4).c_str());
	ifstream wad;
	wad.open(name.c_str(), fstream::in);
	if (wad.is_open()) {
		wad.seekg(0, ios::end);
		int size = wad.tellg(), ie = 0;
		wad.seekg(0, ios::beg);
		char header[12], buffer[size];
		wad.read(header, 12);
		wad.read(buffer, size);
		lumps = get_header_data(4, reinterpret_cast<unsigned char *>(header));
		header_folder = get_header_data(8, reinterpret_cast<unsigned char *>(header));
		char lump[header_folder - 12];
		for (int i = header_folder - 12; i < size; i++) {
			lump[ie] = buffer[i];
			ie++;
		}
		lump_data l;
		l.position = 0;
		l.length = 0;
		vector<lump_data> all_lumps;
		for (int e = 0; e < lumps; e++) {
			std::cout
			<< "\nLump position: " << get_header_data(0 + (e * 16), reinterpret_cast<unsigned char *>(lump)) 
			<< "\nLump size: " << get_header_data(4 + (e * 16), reinterpret_cast<unsigned char *>(lump))
			<< "\nLump name: " << get_lump_name(8 + (e * 16), lump);
			if (get_lump_name(8 + (e * 16), lump).substr(0, 3) != "SS_") {
				l.position = get_header_data(0 + (e * 16), reinterpret_cast<unsigned char *>(lump));
				l.length = get_header_data(4 + (e * 16), reinterpret_cast<unsigned char *>(lump));
				l.name = get_lump_name(8 + (e * 16), lump);
				all_lumps.push_back(l);
			}
		}
		std::experimental::filesystem::create_directories((read_project("file.path") + read_project("project.name")).c_str());
		std::experimental::filesystem::create_directories((read_project("file.path") + read_project("project.name") + "/sprites").c_str());
		for (auto it = all_lumps.begin() ; it != all_lumps.end(); it++) {
			if (it->name == "DECORATE") {
				string path = read_project("file.path") + read_project("project.name");
				std::ofstream resdecorate((path + "/"+it->name+".txt").c_str());
				if (resdecorate.is_open()) {
					for (int lu = it->position - 12; lu < it->length; lu++)
						resdecorate << buffer[lu];
				}
				resdecorate.close();
			} else {
				string path = read_project("file.path") + read_project("project.name") + "/sprites";
				std::ofstream resdecorate((path + "/"+it->name).c_str());
				write_project("states.sprites_names", (read_project("states.sprites_names") + it->name + ";").c_str());
				if (resdecorate.is_open()) {
					for (int lu = it->position - 12; lu < it->length + it->position - 12; lu++)
						resdecorate << buffer[lu];
				}
				resdecorate.close();
			}
		}
	}
	wad.close();
	parse_decorate();
	parse_zscript();
}

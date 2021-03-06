#include <string>
#include <fstream>
#include <iostream>

#include "files.h"
#include "leng.h"
#include "zscript.h"
#include "decorate.h"

std::string decorate_format () {//read data of wad file and writes the TMP file
	std::string props = "", anim = "";
	string state = "";
	if (read_project("states.states_names").length() > 2) {
		for (int i = 0; i < read_project("states.states_names").length(); i++) { 
			if (read_project("states.states_names").at(i) == ';') {
				string line, res = "";
				anim += ("\t\t"+state+":\n");
				ifstream file;
				file.open(string("./config/animations/")+state, fstream::in);
				if (file.is_open()) {
					while (getline (file, line)) {
						anim += ("\t\t\t"+line+"\n");
					}
				}
				file.close();
				state = "";
			} else {
				state += read_project("states.states_names").at(i);
			}
		}
	}
	if (read_project("class.props").length() > 2) {
		for (int i = 0; i < (read_project("class.props").length() - 1); i++) {
			if (read_project("class.props").at(i) == '\\')
				props += "\n\t";
			else
				props += read_project("class.props").at(i);
		}
	}
	std::string res = "actor " 
	+ read_project("class.name")
	+ " "
	+ (read_project("class.replaces").length() > 2 ? 
		"replaces " + read_project("class.replaces") + " "
		: "")
	+ (read_project("class.derivates").length() > 2 ? 
		": " + read_project("class.derivates") + " "
		: "")
	+ "{\n\t" + 
		props + "\n\t" + read_project("class.flag.combo") + 
	"\n\tstates {\n"+
		anim
	+"\t}\n}";
	cout << "\n\n" << res << "\n\n";
	return res;
}

void data_decorate (ifstream file) {}

void parse_decorate () {//create decorate
	ofstream new_file;
	ifstream file_a, file_b, file_c, file_d, file, tmp_file;
	string path = (read_project("file.path") + read_project("project.name")), respath;
	file_a.open(path + "/DECORATE.txt", fstream::in);
	file_b.open(path + "/DECORATE", fstream::in);
	file_c.open(path + "/decorate.txt", fstream::in);
	file_d.open(path + "/decorate", fstream::in);
	if (file_a.good() || file_b.good() || file_c.good() || file_d.good()) {
		if (file_a.good())
			respath = path + "/DECORATE.txt";
		if (file_b.good())
			respath = path + "/DECORATE";
		if (file_c.good())
			respath = path + "/decorate.txt";
		if (file_d.good())
			respath = path + "/decorate";
	}
	file_a.close();
	file_b.close();
	file_c.close();
	file_d.close();
	file.open(respath, fstream::in);
	string line, anim_name;
	bool anim = false;
	if (file.is_open()) {
		while (getline (file, line)) {
			if (line.length() > 11) {
				if (line.find("#include") != std::string::npos) {
					cout << line.substr(10, line.length() - 1);
					//data_decorate();
				}
			}
			if (line.at(line.length() - 1) == ':') {
				write_project("states.states_names", (read_project("states.states_names") +
				line.substr(2, line.find(':') - 2) + ";").c_str());
				anim_name = "./config/animations/"+line.substr(2, line.find(':') - 2);
				new_file.open("./config/animations/"+line.substr(2, line.find(':') - 2), fstream::out);
				new_file.close();
				anim = true;
				continue;
			}
			if (line.substr(0, 3) == "\t\t\t" && anim) {
				tmp_file.open(anim_name, fstream::out);
				string line_tmp, res;
				if (tmp_file.is_open()) {
					while (getline(tmp_file, line_tmp)) {
						res += line_tmp + "\n";
					}
				}
				new_file.open(anim_name, fstream::out);
				new_file << res;
				new_file << line.substr(3, line.length() - 3);
				new_file.close();
				tmp_file.close();
			} else if (line.substr(0, 3) != "\t\t\t" && anim) {
				anim = false;
			}
			if (line == "\tMonster" || line == "\tProjectile") {
				write_project("class.flag.combo", line.substr(1, line.length()).c_str());
			}
			if (
				(line.substr(0, 2) != "\t\t" && line.substr(0, 3) != "\t\t\t") &&
				(line != "\tStates {" && line.find('}') == std::string::npos) &&
				line.at(0) == '\t'
			) {
				write_project("class.props", (read_project("class.props") + line.substr(1, line.length()) + "\\").c_str());
			}
			if (line.substr(0, 5) == "actor") {
				string ln = "";
				int sw = 0;
				for (int i = 0; i < (line + " ").length(); i++) {
					if ((line + " ").at(i) == ' ') {
						if (ln != "actor" && ln != ":" && ln != "replaces" && ln != "{") {
							switch (sw) {
								case 0:
									write_project("class.name", ln.c_str());
									break;
								case 1:
									write_project("class.derivates", ln.c_str());
									break;
								case 2:
									write_project("class.replaces", ln.c_str());
									break;
							}
							++sw;
						}
						ln = "";
					} else {
						ln += (line + " ").at(i);
					}
				}
			} else {
				
			}
		}
	}
	file.close();
}

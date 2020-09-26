#include <string>
#include <fstream>
#include <iostream>
#include <gtk/gtk.h>

#include "files.h"
#include "leng.h"

using namespace std;

string get_word_by_lang (const char* key) { //get the equivalent of word on ES/EN
  string res, line;
  ifstream file;
  file.open(("./config/"+read_config("Lenguage")+".cfg").c_str(), fstream::in);
  if (file.is_open()) {
    while (getline (file, line)) {
      if (line.substr(0, line.find("=")) == key)
        res = line.substr(line.find("=") + 1, line.length());
    }
  }
  file.close();
  return res;
}

bool change_leng (GtkWidget *w) {
	string l = string(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(w))).substr(0, 2);
	write_config("Lenguage", l.c_str());
	cout << "Lenguage: " << l.c_str() << '\n';
	return true;
}

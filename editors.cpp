#include <string>
#include <fstream>
#include <iostream>
#include <gtk/gtk.h>
#include <stdio.h>
#include <cstdint>
#include <experimental/filesystem>

#include "files.h"
#include "leng.h"
#include "gzdoom.h"
#include "editors.h"
#include "animation.h"
#include "decorate.h"
#include "zscript.h"
#include "set_offset.h"
using namespace std;

GtkBuilder *editor_window = gtk_builder_new();
GtkBuilder *main_editor_window = gtk_builder_new();

void close_editor (GtkWidget *w) { //clears the temp file
	experimental::filesystem::remove_all(read_project("file.path") + "/" + read_project("project.name"));
	write_project("class.props", "");
	write_project("class.name", "");
	write_project("class.derivates", "");
	write_project("class.replaces", "");
	write_project("class.flag.combo", "");
	write_project("file.path", "");
	write_project("file.type", "");
	write_project("project.name", "");
	write_project("class.file.type", "DECORATE");
	string state = "";
	if (read_project("states.states_names").length() > 2) {
		for (int i = 0; i < read_project("states.states_names").length(); i++) { 
			if (read_project("states.states_names").at(i) == ';') {
				remove(("./config/animations/"+state).c_str());
				state = "";
			} else {
				state += read_project("states.states_names").at(i);
			}
		}
	}
	write_project("states.states_names", "");
	write_project("states.sprites_names", "");
	gtk_main_quit();
}

void leng_script (GtkWidget *w) { //check if is Zscript or Decorate
	write_project("class.file.type", gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(w)));
	if (string(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(w))) == "ZSCRIPT") {
		write_project("class.derivates", "Actor");
		gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(editor_window, "derivates")), "Actor");
	} else {
		write_project("class.derivates", "");
		gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(editor_window, "derivates")), "");
	}
}

void save_class (GtkWidget *w) { //generates PK3/WAD file
	write_project("class.name", gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(editor_window, "name"))));
	cout << "Class name: " << gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(editor_window, "name"))) << "\n";
	write_project("class.derivates", gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(editor_window, "derivates"))));
	cout << "Class derivates: " << gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(editor_window, "derivates"))) << "\n";
	write_project("class.replaces", gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(editor_window, "replaces"))));
	cout << "Class replaces: " << gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(editor_window, "replaces"))) << "\n";
	string result;
	if (read_project("class.file.type") != "ZSCRIPT")
		result = decorate_format();
	else
		result = zscript_format();
	ofstream resfile;
	resfile.open((read_project("file.path") + "/" + read_project("project.name") + "/"+read_project("class.file.type")+".txt").c_str(), fstream::out);
	resfile << result;
	resfile.close();
	GtkTextBuffer *dec = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(editor_window, "text_end")));
	gtk_text_buffer_set_text(dec, result.c_str(), -1);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(editor_window, "text_end")), dec);
	if (read_project("file.type") != ".wad") { //zip pk3
		system((string("cd \"")+read_project("file.path")+"\";zip -r \""+
			read_project("project.name") + read_project("file.type")
		+"\" \""+
			read_project("project.name")
		+ "\"").c_str());
	} else { //create wad file
		int len = 1;
		for (int i = 0; i < read_project("states.sprites_names").length(); i++) {
			if (read_project("states.sprites_names").at(i) == ';')
				++len;
		}
		if (len > 1)
			len += 2;
		ofstream wad;
		uint32_t decorate_pos(12 + result.length() + ((len > 1) ? sprites_length() : 0));
		wad.open((read_project("file.path") + "/" + read_project("project.name") + read_project("file.type")).c_str(), fstream::out);
		const unsigned char eibe[4] = {
			static_cast<unsigned char>(((unsigned int)decorate_pos & 0xFF000000) >> 24),
			static_cast<unsigned char>(((unsigned int)decorate_pos & 0x00FF0000) >> 16),
			static_cast<unsigned char>(((unsigned int)decorate_pos & 0x0000FF00) >>  8),
			static_cast<unsigned char>(((unsigned int)decorate_pos & 0x000000FF) >>  0)
		}; 
		const unsigned char start[4] = {
			static_cast<unsigned char>(((unsigned int)len & 0xFF000000) >> 24),
			static_cast<unsigned char>(((unsigned int)len & 0x00FF0000) >> 16),
			static_cast<unsigned char>(((unsigned int)len & 0x0000FF00) >>  8),
			static_cast<unsigned char>(((unsigned int)len & 0x000000FF) >>  0)
		}; 
		//00 - 03
		wad << "PWAD";
		//04 - 07
		wad << static_cast<unsigned char>(start[3]);
		wad << static_cast<unsigned char>(start[2]);
		wad << static_cast<unsigned char>(start[1]);
		wad << static_cast<unsigned char>(start[0]);
		//08 - 0b
		wad << eibe[3];
		wad << eibe[2];
		wad << eibe[1];
		wad << eibe[0];
		wad << result.c_str();
		if (len > 1) {
			string file = "", line = "";
			for (int i = 0; i < read_project("states.sprites_names").length(); i++) {
				if (read_project("states.sprites_names").at(i) == ';') {
					ifstream sprite;
					sprite.open(
						(read_project("file.path") + "/" + read_project("project.name") + "/sprites/" + file).c_str()
					);
					if (sprite.is_open()) {
						sprite.seekg(0, ios::end);
						int size = sprite.tellg();
						sprite.seekg(0, ios::beg);
						char buffer[size];
						sprite.read(buffer, size);
						for (int i = 0; i < size; i++)
							wad << buffer[i];
					}
					sprite.close();
					file = "";
				} else {
					file += read_project("states.sprites_names").at(i);
				}
			}
		}
		wad << static_cast<unsigned char>(12);
		wad << static_cast<unsigned char>(0);
		wad << static_cast<unsigned char>(0);
		wad << static_cast<unsigned char>(0);
		wad << static_cast<unsigned char>(result.length());
		wad << static_cast<unsigned char>(0);
		wad << static_cast<unsigned char>(0);
		wad << static_cast<unsigned char>(0);
		wad << read_project("class.file.type").c_str();
		if (len > 1) {
			wad << static_cast<unsigned char>(result.length() + 12);
			wad << static_cast<unsigned char>(0);
			wad << static_cast<unsigned char>(0);
			wad << static_cast<unsigned char>(0);
			wad << static_cast<unsigned char>(0);
			wad << static_cast<unsigned char>(0);
			wad << static_cast<unsigned char>(0);
			wad << static_cast<unsigned char>(0);
			wad << "SS_START";
			string file = "";
			int size = 0, prev_size = 0;
			for (int i = 0; i < read_project("states.sprites_names").length(); i++) {
				if (read_project("states.sprites_names").at(i) == ';') {
					ifstream sprite;
					sprite.open(
						(read_project("file.path") + "/" + read_project("project.name") + "/sprites/" + file)
					, fstream::in);
					if (sprite.is_open()) {
						sprite.seekg(0, ios::end);
						size = sprite.tellg();
						sprite.seekg(0, ios::beg);
					}
					unsigned char beei[4] = {
						static_cast<unsigned char>(((unsigned int)size & 0xFF000000) >> 24),
						static_cast<unsigned char>(((unsigned int)size & 0x00FF0000) >> 16),
						static_cast<unsigned char>(((unsigned int)size & 0x0000FF00) >>  8),
						static_cast<unsigned char>(((unsigned int)size & 0x000000FF) >>  0)
					};
					unsigned char ennd[4] = {
						static_cast<unsigned char>(((unsigned int)result.length() + 12 + prev_size & 0xFF000000) >> 24),
						static_cast<unsigned char>(((unsigned int)result.length() + 12 + prev_size & 0x00FF0000) >> 16),
						static_cast<unsigned char>(((unsigned int)result.length() + 12 + prev_size & 0x0000FF00) >>  8),
						static_cast<unsigned char>(((unsigned int)result.length() + 12 + prev_size & 0x000000FF) >>  0)
					};
					sprite.close();
					wad << static_cast<unsigned char>(ennd[3]);
					wad << static_cast<unsigned char>(ennd[2]);
					wad << static_cast<unsigned char>(ennd[1]);
					wad << static_cast<unsigned char>(ennd[0]);
					wad << beei[3];
					wad << beei[2];
					wad << beei[1];
					wad << beei[0];
					wad << file.c_str();
					prev_size += size;
					for (int ex = file.length(); ex < 8; ex++)
						wad << static_cast<unsigned char>(0);
					file = "";
					size = 0;
				} else {
					file += read_project("states.sprites_names").at(i);
				}
			}
			wad << static_cast<unsigned char>(eibe[3]);
			wad << static_cast<unsigned char>(eibe[2]);
			wad << static_cast<unsigned char>(eibe[1]);
			wad << static_cast<unsigned char>(eibe[0]);
			wad << "\x00\x00\x00\x00";
			wad << "SS_END\x00\x00";
		}
		wad.close();
		std::cout << "\nWAD file created\n";
	}
}

void is_flag (GtkWidget *w) {
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(editor_window, "is_flag"))))
		gtk_widget_show(GTK_WIDGET(gtk_builder_get_object(editor_window, "on_off")));
	else
		gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(editor_window, "on_off")));
}

void remove_prop (GtkWidget *w) {
	GtkWidget *cont = GTK_WIDGET(gtk_builder_get_object(editor_window, "props"));
	const char *text = gtk_button_get_label(GTK_BUTTON(w));
	string data = read_project("class.props");
	write_project("class.props", data.replace(data.find(text) - (data.find(text) > 0 ? 1 : 0 ), string(text).length() + 1, "").c_str());
	gtk_widget_destroy(w);
}

void create_prop (GtkWidget *w) {
	GtkWidget *cont = GTK_WIDGET(gtk_builder_get_object(editor_window, "props"));
	const gchar *text = string(string(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(editor_window, "is_flag"))) ? 
			((gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(editor_window, "on_off")))) ? "+" : "-")
		: "") +
		gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(editor_window, "actor_props"))) + " " +
		gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(editor_window, "prop_argument")))).c_str();
	GtkWidget *np = gtk_button_new_with_label(text);
	gtk_container_add(GTK_CONTAINER(cont), np);
	gtk_widget_show(np);
	write_project("class.props", (read_project("class.props") + text + "\\").c_str());
	g_signal_connect(np, "clicked", G_CALLBACK(remove_prop), editor_window);
}

void editors_inter (GtkWidget *w, GtkBuilder *oringinal_build) {
	write_project("file.type", gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(gtk_builder_get_object(oringinal_build, "file_type"))));
	cout << "File type: " << gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(gtk_builder_get_object(oringinal_build, "file_type"))) << '\n';
	write_project("project.name", gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(oringinal_build, "project_name"))));
	cout << "Project name: " << gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(oringinal_build, "project_name"))) << '\n';
	system(("mkdir \"" + read_project("file.path") + "/" + read_project("project.name") + "\"").c_str());
	system(("mkdir \"" + read_project("file.path") + "/" + read_project("project.name") + "/sprites\"").c_str());
	gtk_builder_add_from_file(main_editor_window, "./glade/editors_inter.glade", NULL);
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(main_editor_window, "GZM_window"));
	gtk_window_set_title(GTK_WINDOW(window), (string("GZM - ")+get_word_by_lang("sett")).c_str());
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(main_editor_window, "arguments_editor")), (get_word_by_lang("argumentos")).c_str());
	gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(main_editor_window, "run_editor")), (get_word_by_lang("run")).c_str());
	gtk_button_set_image(GTK_BUTTON(gtk_builder_get_object(main_editor_window, "animation_editor")),
	gtk_image_new_from_file((get_word_by_lang("animation_editor")).c_str()));
	gtk_button_set_image(GTK_BUTTON(gtk_builder_get_object(main_editor_window, "demon_editor")),
	gtk_image_new_from_file((get_word_by_lang("demon_editor")).c_str()));
	gtk_button_set_image(GTK_BUTTON(gtk_builder_get_object(main_editor_window, "weapon_editor")),
	gtk_image_new_from_file((get_word_by_lang("weapon_editor")).c_str()));
	gtk_button_set_image(GTK_BUTTON(gtk_builder_get_object(main_editor_window, "items_editor")),
	gtk_image_new_from_file((get_word_by_lang("items_editor")).c_str()));
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(main_editor_window, "run_editor")), "clicked", G_CALLBACK(run_mod), NULL);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(main_editor_window, "demon_editor")), "clicked", G_CALLBACK(editors), gtk_label_new("0"));
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(main_editor_window, "weapon_editor")), "clicked", G_CALLBACK(editors), gtk_label_new("1"));
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(main_editor_window, "items_editor")), "clicked", G_CALLBACK(editors), gtk_label_new("3"));
	g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
	gtk_widget_show_all(window);
	gtk_main();
}

void reload_animations (GtkWidget *w) {
	string anim = "";
	GtkWidget *cont = GTK_WIDGET(gtk_builder_get_object(editor_window, "animations_list"));
	GList *children, *iter;
	children = gtk_container_get_children(GTK_CONTAINER(cont));
	for(iter = children; iter != NULL; iter = g_list_next(iter)) {
		gtk_widget_destroy(GTK_WIDGET(iter->data));
	}
	g_list_free(children);
	for (int i = 0; i < read_project("states.states_names").length(); i++) {
		if (read_project("states.states_names").at(i) == ';') {
			GtkWidget *na = gtk_check_button_new_with_label(anim.c_str());
			gtk_container_add(GTK_CONTAINER(cont), na);
			g_signal_connect(na, "toggled", G_CALLBACK(select_animation), editor_window);
			gtk_widget_show(na);
			anim = "";
		} else {
			anim += read_project("states.states_names").at(i);
		}
	}
}

void editors (GtkWidget *w, GtkWidget *fc) {
	gtk_builder_add_from_file(editor_window, "./glade/editor_base.glade", NULL);
	switch (stoi(gtk_label_get_text(GTK_LABEL(fc)))) {
		case 0:
			write_project("class.flag.combo", "Monster");
			break;
		case 1:
			write_project("class.flag.combo", "Weapon");
			break;
		default:
			write_project("class.flag.combo", "");
	}
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(editor_window, "GZM_window"));
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(editor_window, "demon_name")), (get_word_by_lang("name")).c_str());
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(editor_window, "demon_replaces")), (get_word_by_lang("replaces")).c_str());
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(editor_window, "demon_derivates")), (get_word_by_lang("derivates")).c_str());
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(editor_window, "actor_props_label")), (get_word_by_lang("actor_props_label")).c_str());
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(editor_window, "prop_argument_label")), (get_word_by_lang("prop_argument_label")).c_str());
	gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(editor_window, "run_editor")), (get_word_by_lang("run")).c_str());
	gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(editor_window, "is_flag")), (get_word_by_lang("is_flag")).c_str());
	gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(editor_window, "add_prop")), (get_word_by_lang("add_prop")).c_str());
	gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(editor_window, "demon_save")), (get_word_by_lang("save")).c_str());
	gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(editor_window, "on_off")), (get_word_by_lang("on_off")).c_str());
	gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(editor_window, "edi_anim")), (get_word_by_lang("edit")).c_str());
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(editor_window, "add_anim")), "clicked", G_CALLBACK(create_animation), NULL);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(editor_window, "edi_anim")), "clicked", G_CALLBACK(create_animation), NULL);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(editor_window, "pla_anim")), "clicked", G_CALLBACK(play_anim), NULL);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(editor_window, "add_prop")), "clicked", G_CALLBACK(create_prop), NULL);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(editor_window, "rel_anim")), "clicked", G_CALLBACK(reload_animations), NULL);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(editor_window, "is_flag")), "toggled", G_CALLBACK(is_flag), NULL);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(editor_window, "demon_save")), "clicked", G_CALLBACK(save_class), NULL);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(editor_window, "run_editor")), "clicked", G_CALLBACK(run_mod), NULL);
	g_signal_connect(GTK_COMBO_BOX_TEXT(gtk_builder_get_object(editor_window, "de_zs")), "changed", G_CALLBACK(leng_script), NULL);
	gtk_widget_set_name(GTK_WIDGET(gtk_builder_get_object(editor_window, "rem_anim")), "");
	gtk_widget_set_name(GTK_WIDGET(gtk_builder_get_object(editor_window, "edi_anim")), "");
	gtk_widget_set_name(GTK_WIDGET(gtk_builder_get_object(editor_window, "add_anim")), "");
	string prop = "";
	if (read_project("class.props").length() > 2) {
		for (int i = 0; i < read_project("class.props").length(); i++) {
			if (read_project("class.props").at(i) == '\\') {
				GtkWidget *cont = GTK_WIDGET(gtk_builder_get_object(editor_window, "props"));
				GtkWidget *np = gtk_button_new_with_label(prop.c_str());
				gtk_container_add(GTK_CONTAINER(cont), np);
				g_signal_connect(np, "clicked", G_CALLBACK(remove_prop), NULL);
				prop = "";
			} else {
				prop += read_project("class.props").at(i);
			}
		}
	}
	gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(editor_window, "name")), read_project("class.name").c_str());
	gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(editor_window, "replaces")), read_project("class.replaces").c_str());
	gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(editor_window, "derivates")), read_project("class.derivates").c_str());
	g_signal_connect(window, "delete-event", G_CALLBACK(close_editor), NULL);
	gtk_widget_show_all(window);
	gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(editor_window, "on_off")));
	gtk_main();
}

#include <string>
#include <fstream>
#include <iostream>
#include <gtk/gtk.h>

#include "files.h"
#include "leng.h"
#include "gzdoom.h"
#include "editors.h"
#include "load_pk3.h"
#include "load_wad.h"

void set_project_path (GtkWidget *p, GtkWidget *window) {
	GtkWidget *w = gtk_file_chooser_dialog_new((get_word_by_lang("file_path_label")).c_str(),
	GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, "gtk-cancel", GTK_RESPONSE_CANCEL, "gtk-open", GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(w), g_get_home_dir());
	int Diag = gtk_dialog_run(GTK_DIALOG(w));
	if (Diag == GTK_RESPONSE_ACCEPT) {
		char *name = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(w));
		write_project("file.path", name);
		cout << "File path: " << name << '\n';
		gtk_button_set_label(GTK_BUTTON(p), name);
	}
	gtk_widget_destroy(w);
}

void set_gzdoom (GtkWidget *w) { //set GZDoom executable path
	write_config("GZDoom.path", gtk_entry_get_text(GTK_ENTRY(w)));
	cout << "GZDoom path: " << gtk_entry_get_text(GTK_ENTRY(w)) << '\n';
}

void on_new_mod_clicked (GtkWidget *w) {
	GtkBuilder *builder;
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "./glade/create_project_window.glade", NULL);
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "GZM_window"));
	gtk_window_set_title(GTK_WINDOW(window), (string("GZM - ")+get_word_by_lang("new_mod")).c_str());
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "new_name")), (get_word_by_lang("new_mod")).c_str());
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "project_name_label")), (get_word_by_lang("project_name_label")).c_str());
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "file_type_label")), (get_word_by_lang("file_type_label")).c_str());
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "file_path_label")), (get_word_by_lang("file_path_label")).c_str());
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "ok_project")), "clicked", G_CALLBACK(editors_inter), builder);
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "file_path")), "clicked", G_CALLBACK(set_project_path), window);
	g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
	gtk_widget_show_all(window);
	gtk_main();
}

void run_mod (GtkWidget *w, GtkBuilder *b) {
	system(("bash " + 
		read_config("GZDoom.path") +
		" -iwad " + gtk_entry_get_text(
			GTK_ENTRY(gtk_builder_get_object(b, "iwad_label"))
		) +
		" " + gtk_entry_get_text(
			GTK_ENTRY(gtk_builder_get_object(b, "argument_label"))
		) +
		" -file " +
		read_project("file.path") + "/" +
		read_project("project.name") +
		read_project("file.type")
	).c_str());
	std::cout << 
		read_project("file.path") +
		read_project("project.name") +
		read_project("file.type");
}

void on_load_mod_clicked (GtkWidget *p, GtkWidget *window) { //execute GZDoom
	GtkWidget *w = gtk_file_chooser_dialog_new((get_word_by_lang("load_mod")).c_str(),
	GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_OPEN, "gtk-cancel", GTK_RESPONSE_CANCEL, "gtk-open", GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(w), g_get_home_dir());
	int Diag = gtk_dialog_run(GTK_DIALOG(w));
	if (Diag == GTK_RESPONSE_ACCEPT) {
		string name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(w));
		write_project("file.path", name.c_str());
		write_project("file.type", name.substr(name.length() - 4, name.length()).c_str());
		write_project("project.name", g_path_get_basename(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(w))));
		cout << "File name: " << g_path_get_basename(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(w))) << '\n';
		cout << "File type: " << name.substr(name.length() - 4, name.length()) << '\n';
		cout << "File path: " << name << '\n';
		if (string(name.substr(name.length() - 4, name.length())) == ".pk3" 
		 || string(name.substr(name.length() - 4, name.length())) == ".PK3" 
		 || string(name.substr(name.length() - 4, name.length())) == ".zip" 
		 || string(name.substr(name.length() - 4, name.length())) == ".ZIP"
		) {
			load_pk3(name);
		} else {
			load_wad(name);
		}
	}
	gtk_widget_destroy(w);
	GtkWidget *v;
	if (read_project("class.flag.combo") == "Weapon")
		v = gtk_label_new("1");
	else if (read_project("class.flag.combo") == "Monster")
		v = gtk_label_new("0");
	else
		v = gtk_label_new("3");
	editors(gtk_button_new(), v);
}

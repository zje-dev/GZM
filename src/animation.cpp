#include <string>
#include <fstream>
#include <iostream>
#include <gtk/gtk.h>
#include <vector>
#include <bits/stdc++.h>

#include "files.h"
#include "leng.h"
#include "animation.h"
#include "set_offset.h"
#include "editors.h"

void play_anim (GtkWidget *w, GtkBuilder *build) {
	if (string(gtk_widget_get_name(w)).length() > 3) {
		system((string("./prev \"")+gtk_widget_get_name(w)+"\"").c_str()); //runs the animation previsualition program
	}
}

void change_tics (GtkWidget *w, GtkBuilder *builder) { //change the tics of a frame
	if (string(gtk_widget_get_name(GTK_WIDGET(gtk_builder_get_object(builder, "rem_sprite")))) != "@") {
		string new_val = gtk_widget_get_name(GTK_WIDGET(gtk_builder_get_object(builder, "rem_sprite"))),
		tics = "";
		int space = 0;
		for (int i = 0; i < new_val.length(); i++) {
			if (new_val.at(i) == ' ')
				++space;
			if (space < 3 && space > 1)
				tics += new_val.at(i);
		}
		new_val.replace(new_val.find(tics), 
		tics.length(), 
		(" "+ 
			to_string(
				(int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "tics")))
			)
		));
		re_frame(
			gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "state_name"))), //the name of the state
			gtk_widget_get_name(GTK_WIDGET(gtk_builder_get_object(builder, "rem_sprite"))),
			new_val, //the new value
			builder
		);
	}
}

void change_argument (GtkWidget *w, GtkBuilder *builder) {
	if (string(gtk_widget_get_name(GTK_WIDGET(gtk_builder_get_object(builder, "rem_sprite")))) != "@") {
		string new_val = gtk_widget_get_name(GTK_WIDGET(gtk_builder_get_object(builder, "rem_sprite"))),
		arguments = "";
		if (new_val.find('(') != std::string::npos && new_val.find(')') != std::string::npos) { //check if was args
			arguments = new_val.substr(new_val.find('(') + 1, new_val.find(')') - 1);
		}
		new_val.replace(new_val.find(arguments), arguments.length(), string(
			gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "argument")))
		) + ")");
		re_frame(
			gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "state_name"))),
			gtk_widget_get_name(GTK_WIDGET(gtk_builder_get_object(builder, "rem_sprite"))),
			new_val,
			builder
		);
	}
}

void change_funcs (GtkWidget *w, GtkBuilder *builder) {
	if (string(gtk_widget_get_name(GTK_WIDGET(gtk_builder_get_object(builder, "rem_sprite")))) != "@") {
		string new_val = gtk_widget_get_name(GTK_WIDGET(gtk_builder_get_object(builder, "rem_sprite"))),
		functions = "";
		int space = 0;
		for (int i = 0; i < new_val.length(); i++) {
			if (new_val.at(i) == '(' || new_val.at(i) == ' ')
				++space;
			if (space < 4 && space > 2)
				functions += new_val.at(i);
		}
		new_val.replace(new_val.find(functions), functions.length(), string(" ") + gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "funcs"))));
		re_frame(
			gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "state_name"))),
			gtk_widget_get_name(GTK_WIDGET(gtk_builder_get_object(builder, "rem_sprite"))),
			new_val,
			builder
		);
	}
}

void sel_frame (GtkWidget *w, GtkBuilder *builder) { //select frame
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) {
		gtk_widget_set_name(GTK_WIDGET(gtk_builder_get_object(builder, "rem_sprite")), gtk_button_get_label(GTK_BUTTON(w)));
	} else {
		gtk_widget_set_name(GTK_WIDGET(gtk_builder_get_object(builder, "rem_sprite")), "@");
	}
	string label = gtk_button_get_label(GTK_BUTTON(w)),
	label_name = "",
	tics = "0",
	function = "",
	arguments = "";
	int space = 0;
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) {
		for (int i = 0; i < label.length(); i++) {
			if (space < 2) {
				if (label.at(i) == ' ')
					++space;
				else
					label_name += label.at(i);
				tics = "";
			} else if (space < 3) {
				tics += label.at(i);
				if (label.at(i) == ' ')
					++space;
			} else if (space < 4) {
				if (label.at(i) == '(')
					++space;
				function += label.at(i);
			}
			if (label.find('(') != std::string::npos && label.find(')') != std::string::npos) {
				arguments = label.substr(label.find('(') + 1, label.find(')') - 1);
			}
		}
	}
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "tics")), stoi(tics));
	gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "name")), label_name.c_str());
	gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "argument")), (arguments.substr(0, arguments.length() - 2)).c_str());
	gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "funcs")), (function.substr(0, function.length() - 1)).c_str());
}

void rem_frame (GtkWidget *w, GtkBuilder *builder) { //remove frame
	if (string(gtk_widget_get_name(w)) != "@") {
		remove_frame(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "state_name"))), gtk_widget_get_name(w));
		GList *children, *iter;
		children = gtk_container_get_children(GTK_CONTAINER(gtk_builder_get_object(builder, "sprites")));
		for(iter = children; iter != NULL; iter = g_list_next(iter)) {
			if (string(gtk_button_get_label(GTK_BUTTON(iter->data))) == string(gtk_widget_get_name(w)))
				gtk_widget_destroy(GTK_WIDGET(iter->data));
		}
		g_list_free(children);
	}
}

void add_frame (GtkWidget *w, GtkBuilder* builder) { //create frame
	const char *func = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "funcs")));
	string name = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "name")));
	string new_name = (name.substr(0, 4) 
	+ " " + name.substr(4, name.length()) 
	+ " " + to_string(
		(int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "tics")))
	) +
	" " + func
	+ (
		string(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "argument")))).length() > 1 ? 
			(
				string("(") + gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "argument"))) + ")"
			) 
		: ""
	) + "\n");
	if (read_project("states.states_names")
		.find(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "state_name")))) 
		== std::string::npos
	) {
		write_project("states.states_names", (read_project("states.states_names") + gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "state_name"))) + ";").c_str());
	}
	GtkWidget *test = gtk_check_button_new_with_label(new_name.c_str());
	write_frame(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "state_name"))), new_name.c_str());
	gtk_container_add(GTK_CONTAINER(gtk_builder_get_object(builder, "sprites")), test);
	gtk_widget_show(test);
	g_signal_connect(test, "toggled", G_CALLBACK(sel_frame), builder);
}

void create_animation (GtkWidget *w) {
	GtkBuilder *builder;
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "./glade/animation_editor.glade", NULL);
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "GZM_anim"));
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "argument_anim")), (get_word_by_lang("argumentos")).c_str());
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "funs_label")), (get_word_by_lang("funs_label")).c_str());
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "sprite_name")), (get_word_by_lang("sprite_name")).c_str());
	gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder, "add_sprite")), (get_word_by_lang("add_sprite")).c_str());
	gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder, "rem_sprite")), (get_word_by_lang("rem_sprite")).c_str());
	gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder, "edi_anim")), (get_word_by_lang("edit")).c_str());
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "end")), (get_word_by_lang("end")).c_str());
	gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder, "end_ok")), "+");
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "add_sprite")), "clicked", G_CALLBACK(add_frame), builder);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "rem_sprite")), "clicked", G_CALLBACK(rem_frame), builder);
	g_signal_connect(GTK_ENTRY(gtk_builder_get_object(builder, "argument")), "changed", G_CALLBACK(change_argument), builder);
	g_signal_connect(GTK_ENTRY(gtk_builder_get_object(builder, "funcs")), "changed", G_CALLBACK(change_funcs), builder);
	g_signal_connect(GTK_ENTRY(gtk_builder_get_object(builder, "tics")), "changed", G_CALLBACK(change_tics), builder);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "0_button")), "clicked", G_CALLBACK(import_sprite), builder);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "135_button")), "clicked", G_CALLBACK(import_sprite), builder);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "270_button")), "clicked", G_CALLBACK(import_sprite), builder);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "45_button")), "clicked", G_CALLBACK(import_sprite), builder);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "180_button")), "clicked", G_CALLBACK(import_sprite), builder);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "315_button")), "clicked", G_CALLBACK(import_sprite), builder);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "90_button")), "clicked", G_CALLBACK(import_sprite), builder);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "225_button")), "clicked", G_CALLBACK(import_sprite), builder);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "end_ok")), "clicked", G_CALLBACK(set_end), builder);
	gtk_widget_set_name(GTK_WIDGET(gtk_builder_get_object(builder, "rem_sprite")), "@");
	if (string(gtk_widget_get_name(w)).length() > 1) {
		ifstream file;
		string line;
		file.open(string("./config/animations/")+gtk_widget_get_name(w), fstream::in);
		if (file.is_open()) {
			while (getline (file, line)) {
				GtkWidget *test = gtk_check_button_new_with_label(line.c_str());
				write_frame(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "state_name"))), line.c_str());
				gtk_container_add(GTK_CONTAINER(gtk_builder_get_object(builder, "sprites")), test);
				gtk_widget_show(test);
				g_signal_connect(test, "toggled", G_CALLBACK(sel_frame), builder);
			}
		}
		file.close();
		gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "state_name")), gtk_widget_get_name(w));
	}
	g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
	gtk_widget_show_all(window);
	gtk_main();
}

void import_sprite (GtkWidget *w, GtkBuilder *build) {
	GtkWidget *sf = gtk_file_chooser_dialog_new ("Open File", GTK_WINDOW(gtk_builder_get_object(build, "GZM_anim")), GTK_FILE_CHOOSER_ACTION_OPEN,"_Cancel", GTK_RESPONSE_CANCEL,"_Open", GTK_RESPONSE_ACCEPT,NULL);
	if (gtk_dialog_run(GTK_DIALOG(sf)) == GTK_RESPONSE_ACCEPT) {
		char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(sf));
		string name = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(build, "name")));
		if (name.length() > 3) {
			string resname = name.substr(0, 5);
			if (string(gtk_widget_get_name(w)) == "0") {
				if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(build, "mirror")))) {
					resname += "1";
				} else {
					resname += string(gtk_widget_get_name(w));
				}
			} else {
				resname += string(gtk_widget_get_name(w));
			}
			if ((string(gtk_widget_get_name(w)) != "0" && string(gtk_widget_get_name(w)) != "5") && 
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(build, "mirror")))) {
				resname += name.at(4) + to_string(10 - stoi(string(gtk_widget_get_name(w))));
			}
			std::cout << resname << "\n";
			GdkPixbuf *pb = gtk_image_get_pixbuf(GTK_IMAGE(gtk_image_new_from_file(filename)));
			check_and_write(filename, (read_project("file.path") + "/" + read_project("project.name") + "/sprites/" + resname).c_str(), 
				int((float)gdk_pixbuf_get_width(pb) / 2.0f),
				(int)gdk_pixbuf_get_height(pb)
			);
			write_project("states.sprites_names", (read_project("states.sprites_names") + resname + ";").c_str());
		}
	}
	gtk_widget_destroy(sf);
}

void set_end (GtkWidget *w, GtkBuilder *build) {
	write_frame(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(build, "state_name"))),
	gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(build, "end_text"))));
}

void select_animation (GtkWidget *w, GtkBuilder *build) {
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) {
		gtk_widget_set_name(GTK_WIDGET(gtk_builder_get_object(build, "rem_anim")), gtk_button_get_label(GTK_BUTTON(w)));
		gtk_widget_set_name(GTK_WIDGET(gtk_builder_get_object(build, "edi_anim")), gtk_button_get_label(GTK_BUTTON(w)));
		gtk_widget_set_name(GTK_WIDGET(gtk_builder_get_object(build, "pla_anim")), gtk_button_get_label(GTK_BUTTON(w)));
	} else {
		gtk_widget_set_name(GTK_WIDGET(gtk_builder_get_object(build, "rem_anim")), "");
		gtk_widget_set_name(GTK_WIDGET(gtk_builder_get_object(build, "edi_anim")), "");
		gtk_widget_set_name(GTK_WIDGET(gtk_builder_get_object(build, "pla_anim")), "");
	}
}

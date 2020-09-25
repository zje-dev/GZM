#include <iostream>
#include <string>
#include <fstream>
#include <gtk/gtk.h>
#include <vector>
#include <bits/stdc++.h>

#ifdef _WIN32
	#include <Windows.h>
#else
	#include <unistd.h>
#endif

#include "files.h"

//true

GtkBuilder *animation_window;
char direction_animation_window = '1', base_animation_window = 'A';
std::string current_sprite_animation(""), reflect("");

bool check_if_sprite_exists (std::string sprite) {
	return read_project("states.sprites_names").find(sprite) != std::string::npos;
}

static int stop_animation_prev (void*) {
	gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(animation_window, "s")), "[ ► ]");
	return 0;
}

static int update (void *data) {
	std::string *sprites = (std::string*)(data);
	base_animation_window = sprites->at(4);
	if (reflect.length() > 0) {
		reflect = base_animation_window + std::to_string(10 - ((int)direction_animation_window - '0'));
		if (direction_animation_window == '5' || direction_animation_window == '1') {
			reflect = "";
		}
	}
	gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(animation_window, "sprites")),
		(read_project("file.path") + read_project("project.name") + "/" +
		 read_project("project.name") + "/sprites/" + sprites->substr(0, 4) + base_animation_window + direction_animation_window + reflect).c_str()
	);
	std::cout << (read_project("file.path") + read_project("project.name") + "/" +
		read_project("project.name") + "/sprites/" + sprites->substr(0, 4) + base_animation_window + direction_animation_window +
		reflect).c_str() << "\n";
	return 0;
}

void play_animation_prev (GtkWidget *w, void *data) {
	gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(animation_window, "s")), "[ = ]");
	std::vector<std::string>* sprites = (std::vector<std::string> *)(data);
	int prev_tic = 0;
	g_timeout_add(0, update, &(sprites->at(0)));
	for (int i = 1; i < sprites->size(); i++) {
		g_timeout_add((sprites->at(i).at(5) - '0') * 35 + prev_tic, update, &sprites->at(i));
		prev_tic += (sprites->at(i).at(5) - '0') * 35;
	}
	g_timeout_add(prev_tic, stop_animation_prev, animation_window);
}

int next_prev_animation_check () {
/*
	1 = Multiple sprites
	2 = Mirrored sprtes
	0 = single sprite
*/
	if (check_if_sprite_exists((current_sprite_animation.substr(0, 5) + char((int)direction_animation_window + 1)).c_str())) {
		if (!check_if_sprite_exists((current_sprite_animation.substr(0, 5) + (char((int)direction_animation_window + 1)) +current_sprite_animation.at(4) + std::to_string(10 - ((int)direction_animation_window + 1 - '0'))).c_str())) {
			return 1;
		} else {
			return 2;
		}
	} else {
		if (check_if_sprite_exists(current_sprite_animation.substr(0, 5)+"0"))
			return 1;
		else
			return 0;
	}
}

void move_animation_play (bool lr) {
/*
	LR 
	Flase = right
	True = left
*/
	switch (next_prev_animation_check()) {
		case 1:
			if (check_if_sprite_exists(current_sprite_animation.substr(0, 4)+base_animation_window+"2"+base_animation_window+"8")) { //if is mirrored
				if (direction_animation_window == '5' && !lr)
					direction_animation_window = '0';
				if (direction_animation_window == '5' && lr) {
					direction_animation_window = '4';
					reflect = base_animation_window + std::to_string(10 - ((int)direction_animation_window - '0'));
				}
			} else { //if is multiple
				if (direction_animation_window == '8' && !lr)
					direction_animation_window = '1';
				if (direction_animation_window == '1' && lr)
					direction_animation_window = '8';
			}
			direction_animation_window = char((int)direction_animation_window + (lr ? -1 : 1));
			break;
		case 2:
			direction_animation_window = char((int)direction_animation_window + (lr ? -1 : 1));
			reflect = base_animation_window + std::to_string(10 - ((int)direction_animation_window - '0'));
			if (direction_animation_window == '0' && lr) {
				direction_animation_window = '5';
				reflect = "";
			}
			break;
	}
	g_timeout_add(0, update, &current_sprite_animation);
}

void left_move_animation_play () {
	move_animation_play(true);
}

void rigth_move_animation_play () {
	move_animation_play(false);
}

int main (int argc, char **argv) {
	gtk_init(&argc, &argv);
	animation_window = gtk_builder_new();
	gtk_builder_add_from_file(animation_window, "./glade/animation_window.glade", NULL);
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(animation_window, "Animation"));
	//read file
	std::vector<std::string> sprites;
	string sprite = "", cur_sprite = "";
	ifstream file;
	file.open(string("./config/animations/")+string(argv[1]), fstream::in);
	if (file.is_open()) {
		while (getline (file, cur_sprite)) {
			if (cur_sprite.length() > 4) {
				sprite = cur_sprite.substr(0, 4) + cur_sprite.at(5) + cur_sprite.at(7);
				sprites.push_back(sprite);
			}
		}
	}
	file.close();
	//load
	std::cout << check_if_sprite_exists(sprites[0].substr(0, 5)+"0");
	if (check_if_sprite_exists(sprites[0].substr(0, 5)+"0"))
		direction_animation_window = '0';
	g_timeout_add(0, update, &sprites[0]);
	int prev_tic = 0;
	for (int i = 1; i < sprites.size(); i++) {
		g_timeout_add((sprites[i].at(5) - '0') * 35 + prev_tic, update, &sprites[i]);
		current_sprite_animation = sprites[i + ((i + 1) < sprites.size() ? 1 : 0)];
		prev_tic += (sprites[i].at(5) - '0') * 35;
	}
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(animation_window, "n")), "clicked", G_CALLBACK(
		rigth_move_animation_play
	), NULL);
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(animation_window, "p")), "clicked", G_CALLBACK(
		left_move_animation_play
	), NULL);
	g_timeout_add(prev_tic, stop_animation_prev, animation_window);
	g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(animation_window, "s")), "clicked", G_CALLBACK(play_animation_prev), &sprites);
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}

#include <string>
#include <fstream>
#include <iostream>
#include <gtk/gtk.h>

#include "files.h"
#include "leng.h"

/*
	This file is for states-handle
*/

void change_anim (GtkWidget*, GtkBuilder*, bool);
void next_frame_anim (GtkWidget*, GtkBuilder*);
void play_anim (GtkWidget*, GtkBuilder*);
void next_frame_anim_key (GtkWidget*, GdkEventKey*, GtkBuilder*);
void prev_frame_anim_key (GtkWidget*, GdkEventKey*, GtkBuilder*);
void change_argument (GtkWidget*, GtkBuilder*);
void change_tics (GtkWidget*, GtkBuilder*);
void change_funcs (GtkWidget*, GtkBuilder*);
void sel_frame (GtkWidget*, GtkBuilder*);
void rem_frame (GtkWidget*, GtkBuilder*);
void add_frame (GtkWidget*, GtkBuilder*);
void create_animation(GtkWidget*);
void import_sprite (GtkWidget*, GtkBuilder*);
void set_end (GtkWidget*, GtkBuilder*);
void select_animation (GtkWidget*, GtkBuilder*);

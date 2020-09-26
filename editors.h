#include <string>
#include <fstream>
#include <iostream>
#include <gtk/gtk.h>

#include "files.h"
#include "leng.h"
#include "gzdoom.h"

/*
	This file is for define the editor prototypes
*/

void close_editor (GtkWidget*);
void leng_script (GtkWidget*, GtkBuilder*);
void save_class (GtkWidget*, GtkBuilder*);
void is_flag (GtkWidget*, GtkBuilder*);
void reload_prop (GtkWidget*, GtkBuilder*);
void remove_prop (GtkWidget*, GtkBuilder*);
void create_prop (GtkWidget*, GtkBuilder*);
void editors_inter (GtkWidget*, GtkBuilder*);
void editors (GtkWidget *, GtkWidget*);

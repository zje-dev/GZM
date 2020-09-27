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
void leng_script (GtkWidget*);
void save_class (GtkWidget*);
void is_flag (GtkWidget*);
void reload_prop (GtkWidget*);
void remove_prop (GtkWidget*);
void create_prop (GtkWidget*);
void editors_inter (GtkWidget*, GtkBuilder*);
void editors (GtkWidget *, GtkWidget*);

#include <string>
#include <fstream>
#include <iostream>
#include <gtk/gtk.h>

#include "files.h"
#include "leng.h"

void set_project_path (GtkWidget*, GtkWidget*);
void set_gzdoom (GtkWidget*);
void on_new_mod_clicked (GtkWidget*);
void run_mod (GtkWidget*, GtkBuilder*);
void on_load_mod_clicked (GtkWidget*, GtkWidget*);

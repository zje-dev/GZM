#include <gtk/gtk.h>
#include <iostream>

#include "files.h"
#include "leng.h"
#include "gzdoom.h"

using namespace std;

void on_gzm_settings_clicked (GtkWidget *w) { // opens the settings window
	GtkBuilder *builder;
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "./glade/settings_window.glade", NULL);
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "GZM_window"));
	gtk_window_set_title(GTK_WINDOW(window), (string("GZM - ")+get_word_by_lang("sett")).c_str());
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "settings_name")), (get_word_by_lang("sett")).c_str());
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lenguage_name")), (get_word_by_lang("leng")).c_str());
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "gzdoom_path")), (get_word_by_lang("gz_path")).c_str());
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "lenguage")), "changed", G_CALLBACK(change_leng), NULL);
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "gzdoom")), "changed", G_CALLBACK(set_gzdoom), NULL);
	if (read_config("Lenguage") == "ES")
		gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(builder, "lenguage")), 1); //set the current lenguage
	g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
	gtk_widget_show_all(window);
	gtk_main();
}

int main (int argc, char **argv) {
	gtk_init(&argc, &argv);
	GtkBuilder *builder;
 	GtkWidget *window;
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "./glade/main_window.glade", NULL);

	window = GTK_WIDGET(gtk_builder_get_object(builder, "GZM_window"));

	gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder, "new_mod")), (get_word_by_lang("new_mod")).c_str());
	gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder, "load_mod")), (get_word_by_lang("load_mod")).c_str());
	gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder, "gzm_settings")), (get_word_by_lang("config")).c_str());
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "gzm_settings")), "clicked", G_CALLBACK(on_gzm_settings_clicked), NULL);
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "new_mod")), "clicked", G_CALLBACK(on_new_mod_clicked), NULL);//open the 'create window' editor
	g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "load_mod")), "clicked", G_CALLBACK(on_load_mod_clicked), window);//runs the file browser for select a WAD or PK3/ZIP file
	g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);//line 10
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}

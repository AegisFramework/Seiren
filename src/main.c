/* main.c
 *
 * Copyright (C) 2017 Aegis Framework
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <gtk/gtk.h>
#include <stdlib.h>
#include <pthread.h>
#include <glib.h>
#include <syslog.h>
#include "text.h"
#include "server.h"

GtkWidget *text;
int threading = FALSE;
char *root = "/opt/lampp/htdocs";
char *server_dir;

void clear () {
	GList *children, *iter;
	children = gtk_container_get_children(GTK_CONTAINER(text));
	for(iter = children; iter != NULL; iter = g_list_next(iter)) {
		gtk_widget_destroy(GTK_WIDGET(iter -> data));
	}
	g_list_free(children);
}

void write_log (char* str) {
	openlog("Seiren", LOG_PID, LOG_USER);
	syslog(LOG_INFO, str);
	GtkWidget *label = gtk_label_new (str);
	gtk_container_add (GTK_CONTAINER (text), label);
	gtk_label_set_line_wrap (GTK_LABEL(label), TRUE);
	gtk_label_set_xalign (GTK_LABEL(label), 0);
	gtk_widget_show(label);
	closelog();
}

void deploy (GtkButton *button, gpointer spinner) {
	pthread_t my_thread;
	clear();
	write_log ("Initializing The Server...");
	pthread_create(&my_thread, NULL, server_init, (void*) gtk_spin_button_get_value_as_int (spinner));
}

void change_root (GtkButton *button, gpointer entry) {
	GtkWidget *dialog;
	GtkFileChooser *chooser;
	gint res;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
	dialog = gtk_file_chooser_dialog_new ("Select Root Directory", NULL, action, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);

	chooser = GTK_FILE_CHOOSER (dialog);

	res = gtk_dialog_run (GTK_DIALOG (dialog));

	if (res == GTK_RESPONSE_ACCEPT) {
		root =  gtk_file_chooser_get_filename (chooser);
		gtk_entry_set_text (entry, root);
	}

	gtk_widget_destroy (dialog);
}

void toggle_button (GtkButton *button, gpointer other) {
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(button))) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(other), FALSE);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(button), TRUE);
	} else {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(other), TRUE);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(button), FALSE);
	}

	if (equal (gtk_button_get_label(GTK_BUTTON(button)), "Single Thread")) {
		threading = FALSE;
	} else {
		threading = TRUE;
	}
}

static void activate (GtkApplication *app, gpointer user_data) {
	GtkWidget *window;
	GtkWidget *spinner, *button;
	GtkWidget *grid;
	GtkWidget *text_view;
	GtkAdjustment *adjustment;
	GtkWidget *scrolled_window;
	GtkWidget *box;
	GtkWidget *root_box;
	GtkWidget *root_entry;
	GtkWidget *root_button;

	// Window
	window = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW (window), "Seiren Web Server");
	gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);

	// Scrolled Window
	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 15);
	gtk_widget_set_hexpand (scrolled_window, TRUE);
	gtk_widget_set_vexpand (scrolled_window, TRUE);

	// Box Container
	text = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_set_border_width (GTK_CONTAINER (text), 10);
	gtk_container_add (GTK_CONTAINER (scrolled_window), text);

	// Toggle Buttons
	GtkWidget *behavior = gtk_label_new ("Behavior:");
	GtkWidget *toggle_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_container_set_border_width (GTK_CONTAINER (toggle_box), 10);

	GtkWidget *single = gtk_toggle_button_new_with_label ("Single Thread");
	GtkWidget *multi = gtk_toggle_button_new_with_label ("Multi Threading");

	gtk_widget_set_hexpand (single, TRUE);

	gtk_widget_set_hexpand (multi, TRUE);

	g_signal_connect (single, "clicked", G_CALLBACK (toggle_button), multi);
	g_signal_connect (multi, "clicked", G_CALLBACK (toggle_button), single);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(single), TRUE);
	threading = FALSE;

	gtk_container_add (GTK_CONTAINER (toggle_box), single);
	gtk_container_add (GTK_CONTAINER (toggle_box), multi);

	// Root Directory
	GtkWidget *root_container = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
	GtkWidget *root_label = gtk_label_new ("Server Root Directory:");
	root_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
	root_entry = gtk_entry_new ();
	gtk_widget_set_hexpand (root_entry, TRUE);

	gtk_entry_set_text(GTK_ENTRY (root_entry), "/opt/lampp/htdocs");
	root_button =  gtk_button_new_with_label ("Change");
	g_signal_connect (root_button, "clicked", G_CALLBACK (change_root), root_entry);
	gtk_widget_set_hexpand (root_button, TRUE);
	gtk_container_add (GTK_CONTAINER (root_box), root_entry);
	gtk_container_add (GTK_CONTAINER (root_box), root_button);

	gtk_container_add (GTK_CONTAINER (root_container), root_label);
	gtk_container_add (GTK_CONTAINER (root_container), root_box);

	// Spin Button
	GtkWidget *spinner_label = gtk_label_new ("Enter the Port:");
	adjustment = gtk_adjustment_new (3000.0, 80.0, 6655350.00, 10.0, 5.0, 0.0);
	spinner = gtk_spin_button_new (adjustment, 1.0, 0);
	gtk_widget_set_hexpand (spinner, TRUE);

	// Deploy Button
	button = gtk_button_new_with_label ("Deploy");
	gtk_widget_set_hexpand (button, TRUE);
	g_signal_connect (button, "clicked", G_CALLBACK (deploy), spinner);

	// Box Container
	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_add (GTK_CONTAINER (box), spinner_label);
	gtk_container_add (GTK_CONTAINER (box), spinner);
	gtk_container_add (GTK_CONTAINER (box), behavior);
	gtk_container_add (GTK_CONTAINER (box), toggle_box);
	gtk_container_add (GTK_CONTAINER (box), root_container);
	gtk_container_add (GTK_CONTAINER (box), button);
	gtk_container_set_border_width (GTK_CONTAINER (box), 10);

	// Grid Container
	grid = gtk_grid_new ();
	gtk_grid_attach (GTK_GRID (grid), box, 0, 0, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), scrolled_window, 1, 0, 3, 1);

	gtk_container_add (GTK_CONTAINER (window), grid);

	server_dir = concatenate (getenv("HOME"), "/.seiren");
	printf("%s\n", server_dir);

	gtk_widget_show_all (window);
}

int main (int argc, char *argv[]) {
	g_autoptr(GtkApplication) app = NULL;
	int status;

	app = gtk_application_new ("com.aegisframework.Seiren", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
	status = g_application_run (G_APPLICATION (app), argc, argv);

	return status;
}
/* main.c
 *
 * Copyright (C) 2017 Diego Islas Ocampo
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

static void activate (GtkApplication *app, gpointer user_data) {
	GtkWidget *window;
	GtkWidget *grid;
	GtkWidget *scrolled_window;
	GtkWidget *box;

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
	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);


	// Grid Container
	grid = gtk_grid_new ();
	gtk_grid_attach (GTK_GRID (grid), box, 0, 0, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), scrolled_window, 1, 0, 3, 1);

	gtk_container_add (GTK_CONTAINER (window), grid);

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

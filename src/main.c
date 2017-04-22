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
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "text.h"

#define SIZE 8
#define MSGSIZE 1024

GtkTextBuffer *buffer;
GtkTextIter iter;

void writeLog (char* str) {
	if (gtk_text_buffer_get_char_count (buffer) > 0) {
		gtk_text_buffer_get_end_iter(buffer , &iter);
		gtk_text_buffer_insert (buffer, &iter, "\n", -1);
		gtk_text_buffer_get_end_iter(buffer , &iter);
		gtk_text_buffer_insert (buffer, &iter, str, strlen(str));

	} else {
		gtk_text_buffer_set_text (buffer, str, strlen(str));
	}

}

int readLine(int s, char *line, int *result_size) {
    int acum=0, size;
    char buffer[SIZE];

    while( (size=read(s, buffer, SIZE)) > 0) {
        if (size < 0) return -1;
        strncpy(line+acum, buffer, size);
        acum += size;
        if(line[acum-1] == '\n' && line[acum-2] == '\r') {
            break;
        }
    }

    *result_size = acum;

    return 0;
}

int writeLine(int s, char *line, int total_size) {
    int acum = 0, size;
    char buffer[SIZE];

    if(total_size > SIZE) {
        strncpy(buffer, line, SIZE);
        size = SIZE;
    } else  {
        strncpy(buffer, line, total_size);
        size = total_size;
    }

    while( (size=write(s, buffer, size)) > 0) {
        if(size<0) return size;
        acum += size;
        if (acum >= total_size) break;

        size = ((total_size-acum)>=SIZE)?SIZE:(total_size-acum)%SIZE;
        strncpy(buffer, line+acum, size);
    }

    return 0;
}

int serve(int s) {
	printf("1");

    char command[MSGSIZE];
    int size, r, nlc = 0, fd, read_bytes;

    // Lee lo que pide el cliente
    while(1) {
        r = readLine(s, command, &size);
        command[size-2] = 0;
        size-=2;
        writeLog(command);


		// Reached end of the Request Header
        if(command[size-1] == '\n' && command[size-2] == '\r') {
            break;
        }
		// Esto esta mal mal mal
		if(strlen(command) == 0) {
			break;
		}
    }
    sleep(1);

    sprintf(command, "HTTP/1.0 200 OK\r\n");
    writeLine(s, command, strlen(command));

    sprintf(command, "Date: Fri, 31 Dec 1999 23:59:59 GMT\r\n");
    writeLine(s, command, strlen(command));

    sprintf(command, "Content-Type: image/jpeg\r\n");
    writeLine(s, command, strlen(command));

    sprintf(command, "Content-Length: 29936\r\n");
    writeLine(s, command, strlen(command));

    sprintf(command, "\r\n");
    writeLine(s, command, strlen(command));

    FILE *fin = fopen("mainiso_forcampus.jpg", "r");
	FILE *fout = fdopen(s, "w");

	struct stat buf;

	/*stat("mainiso?forcampus.jpg", &buf);
	printf("Size -----------> %ld\n", buf.st_size);

	char file[32*1024];
	int suma = 0;
	size = fread(file, 1, 29936, fin);
	printf("Archivo: %d\n", size);

    while( (size=write(s, &file[suma], MSGSIZE)) > 0) {
		suma += size;
		if (suma >= 29936) break;
	}*/
    sync();
}


static void activate (GtkApplication *app, gpointer user_data) {
	GtkWidget *window;
	GtkWidget *spinner, *button;
	GtkWidget *grid;
	GtkWidget *text_view;
	GtkAdjustment *adjustment;
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

	// Text View
	buffer = gtk_text_buffer_new (NULL);
	text_view = gtk_text_view_new_with_buffer (buffer);
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text_view), GTK_WRAP_WORD);
	gtk_container_add (GTK_CONTAINER (scrolled_window), text_view);

	// Spin Button
	GtkWidget *spinner_label = gtk_label_new ("Enter the Port:");
	adjustment = gtk_adjustment_new (80.0, 80.0, 4000.0, 10.0, 5.0, 0.0);
	spinner = gtk_spin_button_new (adjustment, 1.0, 0);
	gtk_widget_set_hexpand (spinner, TRUE);

	// Deploy Button
	button = gtk_button_new_with_label ("Deploy");
	gtk_widget_set_hexpand (button, TRUE);

	// Box Container
	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_add (GTK_CONTAINER (box), spinner_label);
	gtk_container_add (GTK_CONTAINER (box), spinner);
	gtk_container_add (GTK_CONTAINER (box), button);
	gtk_container_set_border_width (GTK_CONTAINER (box), 10);

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

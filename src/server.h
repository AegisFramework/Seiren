/* server.h
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

#ifndef SERVER_H
#define SERVER_H

#define MSGSIZE 1024

struct Request {
	int size;
	char *method;
	char *path;
	char *extension;
	char *status;
	char content[MSGSIZE];
	char* query;
};

struct Response {
	int code;
	long size;
	char *mime;
	char content[MSGSIZE];
	FILE *file;
};

int serve (int s);

void run_php ();

void header (struct Request *request, struct Response *response, int s);

char *get_filename_ext (char *filename);

int writeLine (int s, char *line, int total_size);

int readLine (int s, char *line, int *result_size);

char *content_type (char *extension);

char *error (int error, char *response);

void *server_init (void *port);

#endif
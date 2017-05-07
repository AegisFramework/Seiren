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
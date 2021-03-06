/* server.c
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

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "text.h"
#include "server.h"
#include "main.h"

#define READ 0
#define WRITE 0
#define SIZE 139
#define MSGSIZE 1024

char* content_type(char *extension) {
	// Text
	if (strncmp(extension, "html", 4) == 0) {
		return "Content-Type: text/html\r\n";
	} else if (strncmp(extension, "php", 3) == 0) {
		return "Content-Type: text/html\r\n";
	} else if (strncmp(extension, "css", 3) == 0) {
		return "Content-Type: text/css\r\n";
	} else if (strncmp(extension, "js", 2) == 0) {
		return "Content-Type: text/javascript\r\n";
	}

	// Images
	if (strncmp(extension, "png", 3) == 0) {
		return "Content-Type: image/png\r\n";
	} else if (strncmp(extension, "jpg", 3) == 0) {
		return "Content-Type: image/jpg\r\n";
	} else if (strncmp(extension, "jpeg", 4) == 0) {
		return "Content-Type: image/jpeg\r\n";
	} else if (strncmp(extension, "gif", 3) == 0) {
		return "Content-Type: image/gif\r\n";
	} else if (strncmp(extension, "bmp", 3) == 0) {
		return "Content-Type: image/bmp\r\n";
	} else if (strncmp(extension, "webp", 3) == 0) {
		return "Content-Type: image/webp\r\n";
	} else if (strncmp(extension, "ico", 3) == 0) {
		return "Content-Type: image/x-icon\r\n";
	} else if (strncmp(extension, "svg", 3) == 0) {
		return "Content-Type: image/svg+xml\r\n";
	}

	// Videos
	if (strncmp(extension, "mov", 3) == 0) {
		return "Content-Type: video/quicktime\r\n";
	} else if (strncmp(extension, "mp4", 3) == 0) {
		return "Content-Type: video/mp4\r\n";
	}
	// Audio
	if (strncmp(extension, "mp3", 3) == 0) {
		return "Content-Type: audio/mpeg3\r\n";
	} else if (strncmp(extension, "ogg", 3) == 0) {
		return "Content-Type: audio/ogg\r\n";
	} else if (strncmp(extension, "flac", 4) == 0) {
		return "Content-Type: audio/flac\r\n";
	}

	// Fonts
	if (strncmp(extension, "ttf", 3) == 0) {
		return "Content-Type: font/truetype\r\n";
	} else if (strncmp(extension, "otf", 3) == 0) {
		return "Content-Type: font/opentype\r\n";
	} else if (strncmp(extension, "woff", 4) == 0) {
		return "Content-Type: application/font-woff\r\n";
	}

	// Archives
	if (strncmp(extension, "zip", 3) == 0) {
		return "Content-Type: application/octet-stream\r\n";
	} else if (strncmp(extension, "rar", 3) == 0) {
		return "Content-Type: application/octet-stream\r\n";
	} else if (strncmp(extension, "tar", 3) == 0) {
		return "Content-Type: application/octet-stream\r\n";
	} else if (strncmp(extension, "gz", 2) == 0) {
		return "Content-Type: application/octet-stream\r\n";
	}

	// Files
	if (strncmp(extension, "pdf", 3) == 0) {
		return "Content-Type: application/pdf\r\n";
	}

	return "Content-Type: text/plain\r\n";
}

int readLine(int s, char *line, int *result_size) {
    int acum = 0, size;
    char buffer[SIZE];
	char *post = "POST";
	int flag = 0;
	int i = 0;
	int cont = 0;
	int temp_size;

    while((size=recv(s, buffer, SIZE, 0)) > 0) {
		printf("READ LOOP\n");
        if (size < 0) break;
        strncpy(line+acum, buffer, size);
        acum += size;
		if (i == 0) {
			flag = contains(buffer, post);
		}

		if (contains (buffer, "www-form-urlencoded")) {
			break;
		}

        if (line[acum-1] == '\n' && line[acum-2] == '\r' && !flag) {
			break;
		} else if (line[acum-1] == '\n' && line[acum-2] == '\r' && flag) {
			cont = 1;
			temp_size = size;
		}

		if (cont){
			if (size > temp_size) {

			} else {
				break;
			}
			temp_size = size;
		}

		i++;
    }
	printf("ENDED READ LOOP\n");
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

char *get_filename_ext(char *filename) {
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) {
		return "";
	}
    return dot + 1;
}

void header (struct Request *request, struct Response *response, int s) {
	sprintf(response -> content, "HTTP/1.0 %d OK\r\n", response -> code);
	writeLine(s, response -> content, strlen(response -> content));

	sprintf(response -> content, "Date: Fri, 31 Dec 1999 23:59:59 GMT\r\n");
	writeLine(s, response -> content, strlen(response -> content));

	sprintf(response -> content, response -> mime);
	writeLine(s, response -> content, strlen(response -> content));

	write_log(concatenate(root, request -> path));

	printf("Size -----------> %ld\n", response -> size);

	sprintf(response -> content, "Content-Length: %ld\r\n", response -> size);
	writeLine(s, response -> content, strlen(response -> content));

	sprintf(response -> content, "Connection: close\r\n");
	writeLine(s, response -> content, strlen(response -> content));

	sprintf(response -> content, "Server: Seiren Web Server\r\n");
	writeLine(s, response -> content, strlen(response -> content));

	sprintf(response -> content, "\r\n");
	writeLine(s, response -> content, strlen(response -> content));
	FILE *fout = fdopen(s, "w");
}

void run_php (struct Request *request, char *file, int s) {
	char *head = "HTTP/1.1 200 OK\nServer: Web Server in C\n Connection: close\n";
	send(s, head, strlen(head), 0);
	if (!fork() || threading) {
		dup2(s, STDIN_FILENO);
		dup2(s, STDOUT_FILENO);
		dup2(s, STDERR_FILENO);
		close(s);
		putenv("GATEWAY_INTERFACE=CGI/1.1");
		putenv(concatenate("REQUEST_METHOD=", request -> method));
		putenv("REDIRECT_STATUS=true");
		if (equal("POST", request -> method)) {
			putenv("CONTENT_TYPE=multipart/form-data");
			putenv("CONTENT_DISPOSITION=form-data");
			putenv(concatenate("CONTENT_LENGTH=", "141"));
		}
		putenv(concatenate("QUERY_STRING=", request -> query));
		putenv(concatenate("SCRIPT_FILENAME=", file));
		putenv("SERVER_PROTOCOL=HTTP/1.1");
		putenv("REMOTE_HOST=127.0.0.1");
		execl("/usr/bin/php-cgi", "php-cgi", NULL);
		perror("PHP");
		sleep(1);
		exit(0);
	} else {
		wait(0);
		sync();
	}
}

int serve(int s) {
	struct Request request;
	struct Response response;
	int r;
	int size;

	r = readLine(s, request.content, &request.size);
	request.size -= 2;
    request.content[request.size] = 0;

	printf("%s\n", "-------------- Request Body Start ------------\n");
	int tok = -1;

	// Split request headers into tokens, using space as delimiter
	char* token = split(request.content, " ");

	// Get the following tokens
	while (token != NULL) {
		if (equal("GET", token) || equal("HEAD", token) || equal("POST", token)) {
			// Request method is sent in the first line
			request.method = malloc(strlen(token) + 1);
			strcpy(request.method, token);
			write_log(request.method);
			tok = 1;
		} else if (tok == 1) {

			request.path = malloc(strlen(token) + 1);
			strcpy(request.path, token);
			char *extension = get_filename_ext(request.path);
			request.extension = malloc(strlen(extension) + 1);
			strcpy(request.extension, extension);
			char *mime = content_type (request.extension);
			response.mime = malloc(strlen(mime) + 1);
			strcpy(response.mime, mime);
			write_log(response.mime);
			tok = 2;



			//break;
		} else if (tok > 1) {
			if (contains(token, "=")) {
				printf("%s\n", token);
			}
		}

		if (strncmp("Content-Length:", token, 14) == 0) {
			printf("Length Read\n");
			tok = 3;
		}
		if (tok == 3) {
			size = atoi(token);
			printf("Length Set as %d", size);
		}
		printf("%s\n", token);
		token = split(NULL, " ");
	}

	printf("------- Request Body End -------------\n");

	if (contains(request.path, "?")) {
		char *requested_file = split(request.path, "?");
		char *query = split(NULL, "?");
		request.path = malloc(strlen(requested_file) + 1);
		strcpy(request.path, requested_file);

		request.query = malloc(strlen(query) + 1);
		strcpy(request.query, query);
	}

    sleep(1);

	if (equal(request.method, "GET")) {

		if (!contains(request.path, ".")) {
			request.extension = "html";
			response.mime = "Content-Type: text/html\r\n";
			request.path = concatenate(request.path, "index.html");
		}

		response.file = fopen(concatenate(root, request.path), "r");
		struct stat buf;
		stat(concatenate(root, request.path), &buf);
		response.size = buf.st_size;
		int size = response.size;

		if (response.file) {
			response.code = 200;
			if (equal("php", request.extension)) {
				run_php(&request, concatenate(root, request.path), s);
			} else {
				header (&request, &response, s);
				char file[size];
				response.size = fread(file, 1, size, response.file);
				int suma = 0;
				while( (response.size=write(s, &file[suma], MSGSIZE)) > 0) {
					suma += response.size;
					if (suma >= size) break;
				}
			}

		} else {

			if (contains(request.path, "favicon.ico")) {
				response.code = 200;
				request.extension = "ico";
				response.mime = "Content-Type: image/x-icon\r\n";
				request.path = "/favicon.ico";
			} else {
				response.code = 404;
				request.extension = "html";
				response.mime = "Content-Type: text/html\r\n";
				request.path = "/error/404.html";
			}

			response.file = fopen(concatenate(server_dir, request.path), "r");
			stat(concatenate(server_dir, request.path), &buf);
			response.size = buf.st_size;
			int size = response.size;
			header (&request, &response, s);

			char file[size];
			response.size = fread(file, 1, size, response.file);
			int suma = 0;
			while( (response.size=write(s, &file[suma], MSGSIZE)) > 0) {
				suma += response.size;
				if (suma >= size) break;
			}
		}

	} else if (equal(request.method, "HEAD")) {

		response.file = fopen(concatenate(root, request.path), "r");
		struct stat buf;
		if (response.file) {
			response.code = 200;
			stat(concatenate(root, request.path), &buf);
		} else {
			response.code = 404;
			response.mime = "Content-Type: text/html\r\n";
			request.path = "/error/404.html";
			response.file = fopen(concatenate(server_dir, request.path), "r");
			stat(concatenate(server_dir, request.path), &buf);
		}
		response.size = buf.st_size;
		header (&request, &response, s);
	} else if (equal(request.method, "POST")) {
		run_php(&request, concatenate(root, request.path), s);
	} else {
		struct stat buf;
		response.code = 400;
		response.mime = "Content-Type: text/html\r\n";
		request.path = "/error/400.html";
		response.file = fopen(concatenate(server_dir, request.path), "r");
		stat(concatenate(server_dir, request.path), &buf);
		response.size = buf.st_size;
		int size = response.size;
		header (&request, &response, s);

		char file[size];
		response.size = fread(file, 1, size, response.file);
		int suma = 0;
		while( (response.size=write(s, &file[suma], MSGSIZE)) > 0) {
			suma += response.size;
			if (suma >= size) break;
		}
	}
    sync();
}

void *server_init (void *port){
	int sd, sdo, size, r;
	socklen_t addrlen;
    struct sockaddr_in sin, pin;

    // 1. Crear el socket
    sd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons((uint) port);

    // 2. Asociar el socket a un IP/puerto
    r = bind(sd, (struct sockaddr *) &sin, sizeof(sin));
	if (r < 0) {
		perror("Bind Error");
		write_log("An error ocurred while trying to bind the specified port.");
		return 0;
	}
	write_log("Port binding successful");
    // 3. Configurar el backlog
    listen(sd, 5);

    addrlen = sizeof(pin);
	int forks;
	int status = 0;
	int wpid;

	// 4. aceptar conexión
    while( (sdo = accept(sd, (struct sockaddr *)  &pin, &addrlen)) > 0) {
		// Check what behavior was chosen
		if (threading) {
			if(!fork()) {
				forks++;
				write_log(concatenate("New Client Connection From: ", inet_ntoa(pin.sin_addr)));

		        printf("Puerto %d\n", ntohs(pin.sin_port));
				serve(sdo);
		        close(sdo);
				exit(0);
		    } else {
				wait(0);
			}
		} else {
			write_log(concatenate("New Client Connection From: ", inet_ntoa(pin.sin_addr)));
	        printf("Puerto %d\n", ntohs(pin.sin_port));
			serve(sdo);
	        close(sdo);
		}
	}
    close(sd);
}
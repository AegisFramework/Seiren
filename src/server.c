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

char *error (int error, char *response) {
	switch (error) {
		case 404:
			break;
		default:
			break;

	}
}

char* content_type(char *extension) {
	// Text
	if (strncmp(extension, "html", 4) == 0) {
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
		return "Content-Type: image/jpeg\r\n";
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

	return "Content-Type: text/plain\r\n";
}

int readLine(int s, char *line, int *result_size) {
    int acum=0, size;
    char buffer[SIZE];

    while((size=read(s, buffer, SIZE)) > 0) {
		printf("READ LOOP\n");
        if (size < 0) return -1;
        strncpy(line+acum, buffer, size);
        acum += size;
        if(line[acum-1] == '\n' && line[acum-2] == '\r') {
            break;
        }
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

int header (struct Request *request, struct Response *response, int s) {
	struct stat buf;

	sprintf(response -> content, "HTTP/1.0 %d OK\r\n", response -> code);
	writeLine(s, response -> content, strlen(response -> content));

	sprintf(response -> content, "Date: Fri, 31 Dec 1999 23:59:59 GMT\r\n");
	writeLine(s, response -> content, strlen(response -> content));

	sprintf(response -> content, response -> mime);
	writeLine(s, response -> content, strlen(response -> content));

	write_log(concatenate("/opt/lampp/htdocs", request -> path));
	stat(concatenate("/opt/lampp/htdocs", request -> path), &buf);
	printf("Size -----------> %ld\n", buf.st_size);

	sprintf(response -> content, "Content-Length: %ld\r\n", buf.st_size);
	writeLine(s, response -> content, strlen(response -> content));

	sprintf(response -> content, "\r\n");
	writeLine(s, response -> content, strlen(response -> content));
	FILE *fout = fdopen(s, "w");

	return buf.st_size;
}

void run_php () {
	int message_fd[2][2];
    int i;

    pipe(message_fd[READ]);
    pipe(message_fd[WRITE]);
    close(message_fd[READ][READ]);
    close(message_fd[WRITE][WRITE]);

    dup2(message_fd[READ][WRITE], 1);
    dup2(message_fd[WRITE][READ], 0);

	putenv("REQUEST_METHOD=GET");
	putenv("REDIRECT_STATUS=True");
	putenv("QUERY_STRING=hola=a&mundo=b");
	putenv("SCRIPT_FILENAME=test.php");

	execlp("php-cgi", "php-cgi", "/opt/lampp/htdocs/test.php", 0);
    close(message_fd[READ][WRITE]);
    close(message_fd[WRITE][READ]);

    FILE *fin = fdopen(message_fd[READ][READ], "r");
    FILE *fout = fdopen(message_fd[WRITE][WRITE], "w");

	char buffer[32];
	int size = 0;
	while(1) {
		if(feof(fin)) break;
		size = fread(buffer, 32, 1, fin);
		fwrite(buffer, 32, 1, stdout);
	}
					printf("Done");
	printf("%s", buffer);
}

int serve(int s) {
	struct Request request;
	struct Response response;
	int r;
	int size;

	r = readLine(s, request.content, &request.size);

	request.size -= 2;
    request.content[request.size] = 0;


	printf("%s\n", "-------------- Just Started ------------\n");
	int tok = -1;

	// Split request headers into tokens, using space as delimiter
	char* token = split(request.content, " ");

	// Get the following tokens
	while (token != NULL) {
		if (strncmp("GET", token, 3) == 0) {
			// Request method is sent in the first line
			request.method = malloc(strlen(token) + 1);
			strcpy(request.method, token);
			write_log(request.method);
			tok = 1;
		} else if (strncmp("POST", token, 4) == 0) {
			// Request method is sent in the first line
			request.method = malloc(strlen(token) + 1);
			strcpy(request.method, token);
			write_log(request.method);
			tok = 1;
			printf ("POST Request Received\n");
		} else if(strncmp("HEAD", token, 4) == 0) {
			// Request method is sent in the first line
			request.method = malloc(strlen(token) + 1);
			strcpy(request.method, token);
			write_log(request.method);
			tok = 1;
		} else if (tok == 1) {
			request.path = malloc(strlen(token) + 1);
			strcpy(request.path, token);
			request.extension = content_type (get_filename_ext(request.path));
			response.mime = malloc(strlen(request.extension) + 1);
			strcpy(response.mime, request.extension);
			write_log(response.mime);
			tok = 2;
			printf("Second Step Done\n");
			//break;
		} else if (tok > 1) {
			if (contains(token, '=')) {
				printf("%s\n", token);
			}
		}

		if (strncmp("Content-Length:", token, 14) == 0) {
			printf("Length Read\n");
			tok = 3;
		} if (tok == 3) {

			size = atoi(token);
			printf("Length Set as %d", size);
		}
		printf("%s\n", token);
		token = split(NULL, " ");
	}

	printf("------- Just Ended -------------\n");

    sleep(1);

	if (strncmp(request.method, "GET", 3) == 0) {
		response.file = fopen(concatenate("/opt/lampp/htdocs", request.path), "r");

		if (response.file) {
			response.code = 200;
			if (strncmp(request.extension, "php", 3) == 0) {
				run_php();
			} else {

			}


		} else {
			response.code = 404;
			response.mime = "Content-Type: text/html\r\n";
			request.path = "/app/bin/error/404.html";
			response.file = fopen(request.path, "r");
		}

		int size = header (&request, &response, s);
		char file[size];
		response.size = fread(file, 1, size, response.file);
		printf("Archivo: %d\n", response.size);
		int suma = 0;
		while( (response.size=write(s, &file[suma], MSGSIZE)) > 0) {
			suma += response.size;
			if (suma >= size) break;
		}

	} else if (strncmp(request.method, "HEAD", 4) == 0) {

		response.file = fopen(concatenate("/opt/lampp/htdocs", request.path), "r");

		if (response.file) {
			response.code = 200;
			if (strncmp(request.extension, "php", 3) == 0) {
				run_php();
			} else {

			}


		} else {
			response.code = 404;
			response.mime = "Content-Type: text/html\r\n";
			request.path = "/app/bin/error/404.html";
			response.file = fopen(request.path, "r");
		}

		int size = header (&request, &response, s);
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
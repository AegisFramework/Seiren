struct Request {
	char* method;
	char* path;
	char* mime;
};

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
	}
	return "Content-Type: text/plain\r\n";
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

char *get_filename_ext(char *filename) {
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

int serve(int s) {
	char command[MSGSIZE];
    int size, r, nlc = 0, fd, read_bytes;
	int line = 0;
	struct Request request;

    // Lee lo que pide el cliente
    while(1) {
        r = readLine(s, command, &size);
        command[size-2] = 0;
        size-=2;
        //write_log(command);
		int tok = 0;

		// Split request headers into tokens, using space as delimiter
		char* token = split(command, " ");

		// Get the following tokens
		while (token != NULL) {
			if (tok == 0) {
				// Request method is sent in the first line
				request.method = malloc(strlen(token) + 1);
				strcpy(request.method, token);
				write_log(request.method);
			}

			// The requested asset is set on the second line
			if (tok == 1) {
				request.path = malloc(strlen(token) + 1);
				strcpy(request.path, token);
				char *type = content_type (get_filename_ext(request.path));
				request.mime = malloc(strlen(type) + 1);
				strcpy(request.mime, type);
				write_log(request.path);
				write_log(request.mime);
				break;
			}
			token = split(NULL, " ");
			tok = tok + 1;
		}

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

    FILE *fin = fopen(concatenate("/opt/lampp/htdocs", request.path), "r");


	if (fin) {

		sprintf(command, "HTTP/1.0 200 OK\r\n");
		writeLine(s, command, strlen(command));

		sprintf(command, "Date: Fri, 31 Dec 1999 23:59:59 GMT\r\n");
		writeLine(s, command, strlen(command));

		sprintf(command, request.mime);
		writeLine(s, command, strlen(command));



		struct stat buf;

		write_log(concatenate("/opt/lampp/htdocs", request.path));
		stat(concatenate("/opt/lampp/htdocs", request.path), &buf);
		printf("Size -----------> %ld\n", buf.st_size);

		char file[buf.st_size];
		int suma = 0;
		size = fread(file, 1, buf.st_size, fin);
		sprintf(command, "Content-Length: %ld\r\n", buf.st_size);
		writeLine(s, command, strlen(command));

		sprintf(command, "\r\n");
		writeLine(s, command, strlen(command));
		printf("Archivo: %d\n", size);
		FILE *fout = fdopen(s, "w");

		while( (size=write(s, &file[suma], MSGSIZE)) > 0) {
			suma += size;
			if (suma >= buf.st_size) break;
		}
	} else {
		sprintf(command, "HTTP/1.0 404 NOT_FOUND\r\n");
		writeLine(s, command, strlen(command));

		sprintf(command, "Date: Fri, 31 Dec 1999 23:59:59 GMT\r\n");
		writeLine(s, command, strlen(command));

		sprintf(command, "Content-Type: text/plain\r\n");
		writeLine(s, command, strlen(command));

		sprintf(command, "Content-Length: 0\r\n");
		writeLine(s, command, strlen(command));

		sprintf(command, "\r\n");
		writeLine(s, command, strlen(command));
		write_log ("There was an error opening the file.");
	}


    sync();
}



void *server_init (void *port){
	int sd, sdo, addrlen, size, r;
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
    // 4. aceptar conexión
    while( (sdo = accept(sd, (struct sockaddr *)  &pin, &addrlen)) > 0) {
        if(!fork()) {
			write_log(concatenate("New Client Connection From: ", inet_ntoa(pin.sin_addr)));

            printf("Puerto %d\n", ntohs(pin.sin_port));
			serve(sdo);

            close(sdo);
			exit(0);
        } else {

				wait(0);
		}
	}

    close(sd);
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "fcntl.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/wait.h>

#define SHM_BYTES 5
/*
 * Para probar, usar netcat. Ej:
 *
 *      $ nc localhost 4040
 *      NUEVO
 *      0
 *      NUEVO
 *      1
 *      CHAU
 */

void quit(char *s)
{
	perror(s);
	abort();
}

int U = 0;

/* Funcion para leer la terminal. */
int fd_readline(int fd, char *buf)
{
	int rc;
	int i = 0;

	/*
	 * Leemos de a un caracter (no muy eficiente...) hasta
	 * completar una línea.
	 */
	while ((rc = read(fd, buf + i, 1)) > 0) {
		if (buf[i] == '\n')
			break;
		i++;
	}

	if (rc < 0)
		return rc;

	buf[i] = 0;
	return i;
}

/* Funcion para manejar una conexion */
void handle_conn(int csock)
{
	char buf[200];
	int rc;
    int fd;
	/*Operaciones para acceder a la memoria compartida*/
	fd = shm_open("/BACK", O_RDWR | O_CREAT, 0644);
    ftruncate(fd,SHM_BYTES);
    void* memptr = mmap(NULL, SHM_BYTES, PROT_READ | PROT_WRITE,
    MAP_SHARED, fd, 0);
	while (1) {
		/* Atendemos pedidos, uno por linea */
		rc = fd_readline(csock, buf);
		if (rc < 0)
			quit("read... raro");

		if (!strcmp(buf, "NUEVO")) {
			/* Ingresaron "NUEVO" */
			char reply[20];
			sprintf(reply, "%d\n", *(int*)memptr); /*Ahora el numero se lee de la memoria compartida*/
			*(int*)memptr += 1; /*Ahora el numero se actualiza en la memoria compartida*/
			write(csock, reply, strlen(reply));
		} else if (!strcmp(buf, "CHAU")) {
            /* Ingresaron "CHAU" */
            close(fd);
			munmap(memptr, SHM_BYTES);
            shm_unlink("/BACK");
			exit(0); 
		}
	}
}

/* Funcion para manejar todo */
void wait_for_clients(int lsock)
{
	int csock;
    int fd;
	int wstatus;
	/* Esperamos una conexión, no nos interesa de donde viene */
	csock = accept(lsock, NULL, NULL);
	if (csock < 0)
		quit("accept");
    fd = fork();
    if(fd == 0) /* sos el hijo, atendes al cliente */
		handle_conn(csock);
	/* sos el padre, esperas al hijo */
	// wait(NULL);
	/* cerras la conexion */
	close(csock);
	/* Volvemos a esperar conexiones */
	wait_for_clients(lsock);
}

/* Crea un socket de escucha en puerto 4040 TCP */
int mk_lsock()
{
	struct sockaddr_in sa; /* Estructura de un socket de internet. */
	int lsock;
	int rc;
	int yes = 1;

	/* Crear socket */
	/* socket recibe: 
	- tipo de conexiones con los que puede conectarse 
	- si va a ser seguro o no.
	- protocolo a seguir segun el tipo de conexiones (suele haber solo 1, se usa 0)
	*/
	lsock = socket(AF_INET, SOCK_STREAM, 0);
	if (lsock < 0) /* socket devuelve el file descriptor que abrio, si tuvo un error da -1. */
		quit("socket");

	/* Setear opción reuseaddr... normalmente no es necesario */
	if (setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == 1)
		quit("setsockopt");

	sa.sin_family = AF_INET;
	sa.sin_port = htons(4040); /* convierte el 4040 en su valor de direccion. */
	sa.sin_addr.s_addr = htonl(INADDR_ANY); /* INADDR_ANY vale cero. lo que hace esto es que sa se 
	fije unicamente en las direcciones de la red local. */

	/* Bindear al puerto 4040 TCP, en todas las direcciones disponibles */
	rc = bind(lsock, (struct sockaddr *)&sa, sizeof sa);
	if (rc < 0)
		quit("bind");

	/* Setear en modo escucha */
	rc = listen(lsock, SHM_BYTES);
	if (rc < 0)
		quit("listen");

	return lsock;
}

int main()
{
	int lsock;
	int fd;

	/* Crea un socket de escucha en puerto 4040 TCP */
	lsock = mk_lsock(); 

    fd = shm_open("/BACK", O_RDWR | O_CREAT, 0644);
    ftruncate(fd,SHM_BYTES);
    void* memptr = mmap(NULL, SHM_BYTES, PROT_READ | PROT_WRITE,
    MAP_SHARED, fd, 0);
    *(int*)memptr = 0; /*Inicializo los numeros en la memoria compartida en 0*/

	wait_for_clients(lsock);
	//munmap(memptr, SHM_BYTES);
    //close(fd);
    //shm_unlink("/BACK");

	return 0;
}

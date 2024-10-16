#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "dato.h"
#include "tabla.h"

#define SHM_NAME "/shared_counter"
#define SHM_SIZE sizeof(TablaHash)
#define SHM_FLAG  O_RDWR | O_CREAT

/*
    Implementacion de un servidor que provee un key-value store a sus clientes.
    Las operaciones disponibles son:
     - PUT k v : introduce al store el valor v bajo la clave k. El valor viejo
    para k, si existia, es pisado. El servidor debe responder con OK.
     - DEL k : Borra el valor asociado a la calve k. El servidor debe 
    responder OK.
     - GET k : Busca el valor asociado a la clave k. El servidor debe contestar 
    con OK v si el valor es v, o NOTFOUND si no hay valor asociado a k.
*/

/*
 * Para probar, usar netcat. Ej:
 *
 *      $ nc localhost 4040
 *      PUT 100 10
 *      OK
 *      GET 100
 *      10
 *      DEL 100
 * 		OK
 */

void quit(char *s)
{
	perror(s);
	abort();
}

int U = 0;

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

char* line_arg(char* line) {
	return strtok(line," ");
}

char* line_key(char* line) {
	char* ret = strtok(line," ");
	ret = strtok(NULL," ");
	return ret;
}

char* line_val(char* line) {
	char* ret = strtok(line," ");
	ret = strtok(NULL," ");
	ret = strtok(NULL," ");
	return ret;
}


void handle_conn(int csock)
{
	void* shared_u;
	char buf[200];
	char* str;
	int rc;
	int fd;
	char* operacion;
	char* clave;
	char* valor;
	Dato dato;
	Dato temp;

	while (1) {
		/* Atendemos pedidos, uno por linea */
		rc = fd_readline(csock, buf);
		if (rc < 0)
			quit("read... raro");

		if (rc == 0) {
			/* linea vacia, se cerró la conexión */
			close(csock);
			return;
		}

		str = buf;
		str = strtok(str, " ");
		operacion = str;
		str = strtok(NULL, " ");
		clave = str;
		str = strtok(NULL, " ");
		valor = str;

		if (!strcmp(operacion, "PUT")) {
			/* Debemos poner la el valor en la clave */
			fd = shm_open(SHM_NAME, SHM_FLAG, 0644);
			ftruncate(fd, SHM_SIZE);
			shared_u = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
			
			dato = dato_crear(clave, valor);
			tablahash_insertar(*(TablaHash*)shared_u, dato);
			dato_destruye(dato);
			
			char reply[20];
			// sprintf(reply, "%p\n", shared_u);
			sprintf(reply, "OK\n");
			write(csock, reply, strlen(reply));
		} else if (!strcmp(operacion, "GET")) {
			/* Buscamos el valor de la clave */
			fd = shm_open(SHM_NAME, SHM_FLAG, 0644);
			ftruncate(fd, SHM_SIZE);
			shared_u = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
			
			dato = dato_crear(clave, "0");
			temp = tablahash_buscar(*(TablaHash*)shared_u, dato);
			dato_destruye(dato);
			
			char reply[20];
			if (temp != NULL) {
				sprintf(reply, "OK %d\n", temp->valor);
			} else {
				sprintf(reply, "NOTFOUND\n");
			}
			write(csock, reply, strlen(reply));
		} else if (!strcmp(operacion, "DEL")) {
			/* Borramos la clave del valor */
			fd = shm_open(SHM_NAME, SHM_FLAG, 0644);
			ftruncate(fd, SHM_SIZE);
			shared_u = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
			
			dato = dato_crear(clave, "0");
			tablahash_eliminar(*(TablaHash*)shared_u, dato);
			dato_destruye(dato);

			char reply[20];
			sprintf(reply, "OK\n");
			write(csock, reply, strlen(reply));
		}
	}
}

void wait_for_clients(int lsock)
{
	int csock;

	/* Esperamos una conexión, no nos interesa de donde viene */
	csock = accept(lsock, NULL, NULL);
	if (csock < 0)
		quit("accept");

    /* Creamos un hijo para que atienda el. */
    int pid = fork();
	/* Atendemos al cliente */
    if (pid == 0)
	    handle_conn(csock);
	/* El padre cierra el puerto para que quede nomas el hijo */
	close(csock);
	/* Volvemos a esperar conexiones */
	wait_for_clients(lsock);
}

/* Crea un socket de escucha en puerto 4040 TCP */
int mk_lsock()
{
	struct sockaddr_in sa;
	int lsock;
	int rc;
	int yes = 1;

	/* Crear socket */
	lsock = socket(AF_INET, SOCK_STREAM, 0);
	if (lsock < 0)
		quit("socket");

	/* Setear opción reuseaddr... normalmente no es necesario */
	if (setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == 1)
		quit("setsockopt");

	sa.sin_family = AF_INET;
	sa.sin_port = htons(4040);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Bindear al puerto 4040 TCP, en todas las direcciones disponibles */
	rc = bind(lsock, (struct sockaddr *)&sa, sizeof sa);
	if (rc < 0)
		quit("bind");

	/* Setear en modo escucha */
	rc = listen(lsock, 10);
	if (rc < 0)
		quit("listen");

	return lsock;
}

int main()
{
	int lsock;
	lsock = mk_lsock();

	int fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0644);
	ftruncate(fd, SHM_SIZE);
	void* shared_u = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	*(TablaHash*)shared_u = tablahash_crear(100, dato_copia, dato_compara, dato_destruye, dato_hash);

	wait_for_clients(lsock);

	return 0;
}
#include"utils.h"

t_log* logger;

int iniciar_servidor(void)
{
	// Quitar esta línea cuando hayamos terminado de implementar la funcion
	//assert(!"no implementado!");

	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, PUERTO, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	// log info ip y puerto
	log_info(logger, "IP: %s", servinfo->ai_addr);
	log_info(logger, "Puerto: %s", PUERTO);

	if (socket_servidor == -1)
	{
		log_error(logger, "Error al crear el socket");
		exit(1);
	}
	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	
	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	// Quitar esta línea cuando hayamos terminado de implementar la funcion
	//assert(!"no implementado!");

	// Aceptamos un nuevo cliente
	int socket_cliente;
	
	socket_cliente = accept(socket_servidor, NULL, NULL);
	if (socket_cliente == -1)
	{
		log_error(logger, "Error al aceptar un cliente");
		return -1;
	}

	if(!handshake(socket_cliente))
	{
		close(socket_cliente);
		return -1;
	}
	
	log_info(logger, "Se conecto un cliente!");
	return socket_cliente;
}

bool handshake(int socket_cliente)
{
	int handshake;
	if(recv(socket_cliente, &handshake, sizeof(int), MSG_WAITALL) <= 0) 
	{	// error case
		log_error(logger, "Error al recibir handshake");
		return false;
	}

	if(handshake != 1)
	{
		log_error(logger, "Handshake fallido");
		send(socket_cliente, 0, sizeof(int), 0);
		return false;
	}
	
	log_info(logger, "Handshake exitoso");
	send(socket_cliente, &handshake, sizeof(int), 0);
	return true;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) <= 0)
	{
		close(socket_cliente);
		return -1;
	}

	return cod_op;
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}

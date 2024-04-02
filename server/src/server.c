#include "server.h"

int main(void) {
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);

	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			continue;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "recived: \n");
			list_iterate(lista, (void*) iterator);
			continue;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			log_info(logger, "Servidor listo para recibir al cliente");
			int cliente_fd = esperar_cliente(server_fd);
			continue;
			//return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return EXIT_SUCCESS;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}

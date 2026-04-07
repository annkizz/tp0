#include "utils.h"

t_log* logger;

int iniciar_servidor(void)
{

	int err;
	int socket_servidor;
    int socket_cliente;         // ← subila acá

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	err = getaddrinfo(NULL, PUERTO, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	// Asociamos el socket a un puerto
	err = setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));

	err = bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	// Escuchamos las conexiones entrantes

	err = listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");


	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{

	// Aceptamos un nuevo cliente
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
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

t_buffer2 *buffer_create(uint32_t size) {
    t_buffer2 *buffer = malloc(sizeof(t_buffer2));
    buffer->size = size;
    buffer->offset = 0;
    buffer->stream = malloc(size);
    return buffer;
}

void buffer_destroy(t_buffer2 *buffer) {
    free(buffer->stream);
    free(buffer);
}

void buffer_add(t_buffer2 *buffer, void *data, uint32_t size) {
    memcpy(buffer->stream + buffer->offset, data, size);
    buffer->offset += size;
}

void buffer_read(t_buffer2 *buffer, void *data, uint32_t size) {
    memcpy(data, buffer->stream + buffer->offset, size);
    buffer->offset += size;
}

void buffer_add_uint32(t_buffer2 *buffer, uint32_t data) {
    buffer_add(buffer, &data, sizeof(uint32_t));
}

uint32_t buffer_read_uint32(t_buffer2 *buffer) {
    uint32_t data;
    buffer_read(buffer, &data, sizeof(uint32_t));
    return data;
}

void buffer_add_contexto(t_buffer2 *buffer, t_contexto data) {
	buffer_add(buffer, &data, sizeof(t_contexto));
}

t_contexto buffer_read_contexto(t_buffer2 *buffer) {
	t_contexto data;
	buffer_read(buffer, &data, sizeof(t_contexto));
	return data;
}

t_buffer2 *pcb_serializar(t_pcb *proceso) {
    t_buffer2 *buffer = buffer_create(
      sizeof(uint32_t) * 3 + 
      sizeof(t_contexto)
    );

    buffer_add_uint32(buffer, proceso->id);
    buffer_add_uint32(buffer, proceso->estado);
	buffer_add_uint32(buffer,proceso->prioridad);
    buffer_add_contexto(buffer, proceso->contexto);

    return buffer;
}

t_pcb *proceso_deserializar(t_buffer2 *buffer) {
    t_pcb *proceso = malloc(sizeof(t_pcb));

    proceso->id = buffer_read_uint32(buffer);
    proceso->estado = buffer_read_uint32(buffer);
    proceso->prioridad = buffer_read_uint32(buffer);
	proceso->contexto = buffer_read_contexto(buffer);
    return proceso;
}

t_pcb pcb_crear(uint32_t id, uint32_t estado, t_contexto contexto, int32_t prioridad) {
    t_pcb proceso;
	proceso.id = id;
	proceso.estado = estado;
	proceso.prioridad = prioridad;
	proceso.contexto = contexto;
    return proceso;
}

t_pcb *recibir_paquetePCB(int socket_cliente) {
    // Recibimos el size del buffer
    int size;
    recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);

    // Recibimos el stream
    t_buffer2 *buffer = buffer_create(size);
    recv(socket_cliente, buffer->stream, size, MSG_WAITALL);

    // Deserializamos
    t_pcb *proceso = proceso_deserializar(buffer);

    buffer_destroy(buffer);
    return proceso;
}
#include "utils.h"


void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family,server_info->ai_socktype,server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo
	
	int err = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

	freeaddrinfo(server_info);

	return socket_cliente;
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}


void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete(void)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = PAQUETE;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

t_pcb pcb_crear(uint32_t id, uint32_t estado, t_contexto contexto, int32_t prioridad) {
    t_pcb proceso;
	proceso.id = id;
	proceso.estado = estado;
	proceso.contexto = contexto;
	proceso.prioridad = prioridad;
    return proceso;
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
    proceso->estado = buffer_read_uint32 (buffer);
	proceso->prioridad = buffer_read_uint32(buffer);
	proceso->contexto = buffer_read_contexto(buffer);
    return proceso;
}

void enviar_paquetePCB(t_pcb *proceso, int socket_cliente) {
    t_buffer2 *buffer = pcb_serializar(proceso);

    int cod_op = PCB;
    int bytes = sizeof(int) * 2 + buffer->size;
    void *a_enviar = malloc(bytes);
    int offset = 0;

    memcpy(a_enviar + offset, &cod_op, sizeof(int));
    offset += sizeof(int);
    memcpy(a_enviar + offset, &buffer->size, sizeof(int));
    offset += sizeof(int);
    memcpy(a_enviar + offset, buffer->stream, buffer->size);

    send(socket_cliente, a_enviar, bytes, 0);

    free(a_enviar);
    buffer_destroy(buffer);
}
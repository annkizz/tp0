#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>

typedef enum
{
	MENSAJE,
	PAQUETE,
	PCB
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	int size;
	int offset;
	void* stream;
} t_buffer2;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef struct {
	int32_t AX;
	int32_t BX;
	int32_t IP;
	int32_t CP;
	int32_t PSW;
} t_contexto;

typedef struct {
	int32_t id;
	int32_t estado;
	t_contexto contexto;
	int32_t prioridad;
} t_pcb;

typedef struct {
    uint8_t codigo_operacion;
    t_buffer2* buffer;
} t_paquete2;


int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
t_paquete* crear_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
t_pcb pcb_crear(uint32_t id, uint32_t estado, t_contexto contexto, int32_t prioridad);
void buffer_add_uint32(t_buffer2 *buffer, uint32_t data);
uint32_t buffer_read_uint32(t_buffer2 *buffer);
t_buffer2 *buffer_create(uint32_t size);
void buffer_destroy(t_buffer2 *buffer);
void buffer_add_contexto(t_buffer2 *buffer, t_contexto data);
t_contexto buffer_read_contexto(t_buffer2 *buffer);
t_buffer2 *pcb_serializar(t_pcb *proceso);
t_pcb *proceso_deserializar(t_buffer2 *buffer);
void enviar_paquetePCB(t_pcb *proceso, int socket_cliente);

#endif /* UTILS_H_ */

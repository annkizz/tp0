#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<assert.h>

#define PUERTO "4444"

typedef enum
{
	MENSAJE,
	PAQUETE,
	PCB
}op_code;

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

typedef struct
{
	int size;
	int offset;
	void* stream;
} t_buffer2;

extern t_log* logger;

void* recibir_buffer(int*, int);

int iniciar_servidor(void);
int esperar_cliente(int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);
void conectarCLientes(int socketServidor);
t_pcb pcb_crear(uint32_t id, uint32_t estado, t_contexto contexto, int32_t prioridad);
void buffer_add_uint32(t_buffer2 *buffer, uint32_t data);
uint32_t buffer_read_uint32(t_buffer2 *buffer);
t_buffer2 *buffer_create(uint32_t size);
void buffer_destroy(t_buffer2 *buffer);
void buffer_add_contexto(t_buffer2 *buffer, t_contexto data);
t_contexto buffer_read_contexto(t_buffer2 *buffer);
t_buffer2 *pcb_serializar(t_pcb *proceso);
t_pcb *proceso_deserializar(t_buffer2 *buffer);
t_pcb *recibir_paquetePCB(int socket_cliente);

#endif /* UTILS_H_ */

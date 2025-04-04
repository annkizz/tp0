package main

import (
	//"bufio"
	"client/globals"
	"client/utils"
	"log"
	//"os"
)

func main() {
	utils.ConfigurarLogger()

	log.Println("Hola soy un log") // loggear "Hola soy un log" usando la biblioteca log
	globals.ClientConfig = utils.IniciarConfiguracion("config.json")
	//log.Println("Configuracion iniciada")

	if globals.ClientConfig == nil {
		log.Fatalf("No se pudo cargar la configuración") // validar que la config este cargada correctamente
	}

	log.Println("Mensaje de configuración:", globals.ClientConfig.Mensaje)
	log.Println("Puerto:", globals.ClientConfig.Puerto) // loggeamos el valor de la config
	log.Println("Ip", globals.ClientConfig.Ip)

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	utils.EnviarMensaje("localhost", 8080, globals.ClientConfig.Mensaje) // enviar un mensaje al servidor con el valor de la config

	// leer de la consola el mensaje
	// utils.LeerConsola()

	// generamos un paquete y lo enviamos al servidor

	utils.GenerarYEnviarPaquete()
}


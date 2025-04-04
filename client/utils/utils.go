package utils

import (
	"bufio"
	"bytes"
	"client/globals"
	"encoding/json"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
)

type Mensaje struct {
	Mensaje string `json:"mensaje"`
}

type Paquete struct {
	Valores []string `json:"valores"`
}

func IniciarConfiguracion(filePath string) *globals.Config {
	var config *globals.Config

	configFile, err := os.Open(filePath)
	if err != nil {
		log.Fatal(err.Error())
	}
	defer configFile.Close()

	jsonParser := json.NewDecoder(configFile)
	jsonParser.Decode(&config)

	return config
}

func LeerConsola() Paquete {
	// Leer de la consola
	paquete := Paquete{}

	reader := bufio.NewReader(os.Stdin)

	for {

	log.Println("Ingrese los mensajes")
	text, _ := reader.ReadString('\n')
	log.Print(text)

	if text == "\n" {
		break
	}

	text = text[:len(text)-1]
	paquete.Valores = append(paquete.Valores , text) // me da undefined: paquete pq en generaryenviarpaquete no devuelvo el paquete hecho, 
	//es mas no lo creo, entonces lo debo crear y retornarlo
}
   
    return paquete;

}

func GenerarYEnviarPaquete() {
	//paquete := Paquete{}
	// Leemos y cargamos el paquete

	paquete := LeerConsola()

	log.Printf("paqute a enviar: %+v", paquete)

	EnviarPaquete("localhost", 8080, paquete)
	// Enviamos el paqute
}

func EnviarMensaje(ip string, puerto int, mensajeTxt string) {
	mensaje := Mensaje{Mensaje: mensajeTxt}
	body, err := json.Marshal(mensaje)
	if err != nil {
		log.Printf("error codificando mensaje: %s", err.Error())
	}

	url := fmt.Sprintf("http://%s:%d/mensaje", ip, puerto)
	resp, err := http.Post(url, "application/json", bytes.NewBuffer(body))
	if err != nil {
		log.Printf("error enviando mensaje a ip:%s puerto:%d", ip, puerto)
	}

	log.Printf("respuesta del servidor: %s", resp.Status)
}

func EnviarPaquete(ip string, puerto int, paquete Paquete) {
	body, err := json.Marshal(paquete)
	if err != nil {
		log.Printf("error codificando mensajes: %s", err.Error())
	}

	url := fmt.Sprintf("http://%s:%d/paquetes", ip, puerto)
	resp, err := http.Post(url, "application/json", bytes.NewBuffer(body))
	if err != nil {
		log.Printf("error enviando mensajes a ip:%s puerto:%d", ip, puerto)
	}

	log.Printf("respuesta del servidor: %s", resp.Status)
}

func ConfigurarLogger() {
	logFile, err := os.OpenFile("tp0.log", os.O_CREATE|os.O_APPEND|os.O_RDWR, 0666)
	if err != nil {
		panic(err)
	}
	mw := io.MultiWriter(os.Stdout, logFile)
	log.SetOutput(mw)
}

/*// Creamos un lector de consola
	reader := bufio.NewReader(os.Stdin)

	// Le pedimos al usuario que ingrese un mensaje
	fmt.Println("Ingresa algo:")

	// Leemos lo que el usuario ingresa hasta que presione Enter (\n)
	text, _ := reader.ReadString('\n')*/
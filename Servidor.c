#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>

#define PORT 8080
#define BUFFER_SIZE 1024

#define VOCALES "aeiou"
#define CONSONANTES "bcdfghjklmnpqrstvwxyz"
#define CARACTERES "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

char *generar_nombre_usuario(int longitud) {
    static char nombre[16];

    int es_vocal = rand() % 2;
    int i;

    if (longitud < 5 || longitud > 15) {
        return NULL;
    }

    for (i = 0; i < longitud; i++) {
        if (es_vocal) {
            nombre[i] = VOCALES[rand() % strlen(VOCALES)];
        } else {
            nombre[i] = CONSONANTES[rand() % strlen(CONSONANTES)];
        }
        es_vocal = !es_vocal;
    }
    nombre[longitud] = '\0';

    return nombre;
}

char *generar_clave(int longitud) {
    static char clave[51];
    int i;

    if (longitud < 8 || longitud >= 50) {
        return NULL;
    }

    for (i = 0; i < longitud; i++) {
        clave[i] = CARACTERES[rand() % strlen(CARACTERES)];
    }
    clave[longitud] = '\0';

    return clave;
}

void manejar_cliente(SOCKET cliente_socket) {
    char buffer[BUFFER_SIZE] = {0};
    int longitud;
    char *resultado;

    send(cliente_socket, "Conexion establecida con el servidor.\n", 38, 0);

    while (1) {
        // Enviar el menú de opciones al cliente
        memset(buffer, 0, BUFFER_SIZE);
        send(cliente_socket, "Seleccione una opcion:\n1. Generar usuario\n2. Generar contrasena\n0. Salir\nIngrese la opcion deseada: ", 91, 0);

        // Recibir la opción del cliente
        int bytes_recibidos = recv(cliente_socket, buffer, sizeof(buffer) - 1, 0); // Deja espacio para el \0
        
        if (bytes_recibidos <= 0) {

            printf("Cliente desconectado o error al recibir datos.\n");
            break;
        }
        buffer[bytes_recibidos] = '\0'; // Asegura la terminación del string

        if (strncmp(buffer, "0", 1) == 0) {

            send(cliente_socket, "Saliendo...\n", 12, 0);
            printf("Cliente desconectado .\n");
            break;
        } else if (strncmp(buffer, "1", 1) == 0) {

            send(cliente_socket, "Ingrese la longitud del nombre de usuario (5-15): ", 52, 0);
            bytes_recibidos = recv(cliente_socket, buffer, sizeof(buffer) - 1, 0); // Deja espacio para el \0
            buffer[bytes_recibidos] = '\0'; // Asegura la terminación del string
            longitud = atoi(buffer);
            resultado = generar_nombre_usuario(longitud);

            if (resultado) {
                send(cliente_socket, resultado, strlen(resultado), 0);
            } else {
                send(cliente_socket, "Longitud invalida. Debe estar entre 5 y 15 caracteres.\n", 55, 0);
            }
        } else if (strncmp(buffer, "2", 1) == 0) {

            send(cliente_socket, "Ingrese la longitud de la contrasena (8-50): ", 46, 0);
            bytes_recibidos = recv(cliente_socket, buffer, sizeof(buffer) - 1, 0); // Deja espacio para el \0
            buffer[bytes_recibidos] = '\0'; // Asegura la terminación del string
            longitud = atoi(buffer);
            resultado = generar_clave(longitud);

            if (resultado) {
                send(cliente_socket, resultado, strlen(resultado), 0);

            } else {
                send(cliente_socket, "Longitud invalida. Debe estar entre 8 y 50 caracteres.\n", 55, 0);
            }
        } else {
            send(cliente_socket, "Opcion no valida.\n", 19, 0);
        }
    }

    closesocket(cliente_socket);
}

int main() {
    WSADATA wsa;
    SOCKET server_fd, nuevo_socket;
    struct sockaddr_in direccion;
    int dir_length = sizeof(direccion);

    printf("Inicializando Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Error en WSAStartup. Código de error: %d\n", WSAGetLastError());
        return 1;
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Error al crear el socket. Código de error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&direccion, sizeof(direccion)) == SOCKET_ERROR) {
        printf("Error en el enlace (bind). Código de error: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    if (listen(server_fd, 3) == SOCKET_ERROR) {
        printf("Error en la escucha (listen). Código de error: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Servidor en espera de conexiones...\n");

    while ((nuevo_socket = accept(server_fd, (struct sockaddr *)&direccion, &dir_length)) != INVALID_SOCKET) {
        printf("Conexion aceptada.\n");
        manejar_cliente(nuevo_socket);
    }

    if (nuevo_socket == INVALID_SOCKET) {
        printf("Error al aceptar la conexión. Código de error: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    closesocket(server_fd);
    WSACleanup();

    return 0;
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>

//#pragma comment(lib, "ws2_32.lib") //Para compilar desde vs

#define PORT 8080


int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char opcion[10] = {0};

    printf("Inicializando Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Error en WSAStartup. Código de error: %d\n", WSAGetLastError());
        return 1;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Error al crear el socket. Código de error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Intentando conectar al servidor...\n");

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        printf("Error de conexión. Código de error: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Conectado al servidor exitosamente.\n");

    while (1) {
        // Recibe el mensaje del servidor
        int bytes_recibidos = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_recibidos <= 0) {
            printf("Error al recibir datos del servidor o conexión cerrada.\n");
            break;
        }
        buffer[bytes_recibidos] = '\0'; // Asegurarse de que el buffer esté terminado en nulo
        printf("Mensaje recibido del servidor: %s\n", buffer);

        // Pide al usuario ingresar una opción solo si el servidor pide una opcion
        if (strstr(buffer, "Seleccione una opcion") != NULL || strstr(buffer, "Ingrese la opcion deseada") != NULL) {
            printf("Opcion: ");
            fgets(opcion, sizeof(opcion), stdin);
            opcion[strcspn(opcion, "\n")] = 0; // Remover salto de línea

 

            // Enviar la opción al servidor
            send(sock, opcion, strlen(opcion), 0);

            // Si la opción es "0", salir del bucle
            if (strcmp(opcion, "0") == 0) {
                break;
            }
        }
        // Pide al usuario ingresar la longitud si seleccionó "1" o "2"
        if (strstr(buffer, "Ingrese la longitud") != NULL) {
            printf("Ingrese la longitud: ");
            fgets(opcion, sizeof(opcion), stdin);
            opcion[strcspn(opcion, "\n")] = 0;

            // Enviar la longitud al servidor
            send(sock, opcion, strlen(opcion), 0);

            // Recibe la respuesta final del servidor
            bytes_recibidos = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (bytes_recibidos <= 0) {
                printf("Error al recibir datos del servidor o conexion cerrada.\n");
                break;
            }
            buffer[bytes_recibidos] = '\0';
            printf("Respuesta final del servidor: %s\n", buffer);
        }
    }

    closesocket(sock);
    WSACleanup();

    return 0;
}



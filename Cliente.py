import socket

# Configuración del cliente
HOST = '127.0.0.1'  # Dirección del servidor
PORT = 8080         # Puerto del servidor

def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        
        while True:
            # Recibir mensaje del servidor
            datos = s.recv(1024).decode()
            if not datos:
                print("Conexión cerrada por el servidor.")
                break
            print("Mensaje recibido del servidor:", datos)
            
            # Ingresar la opción deseada
            if "Seleccione una opcion" in datos or "Ingrese la opcion deseada" in datos:
                opcion = input("Opción: ")
                s.sendall(opcion.encode())
                
                # Si la opción es "0", salir del bucle
                if opcion == "0":
                    break
            
            # Ingresar la longitud si se solicita
            elif "Ingrese la longitud" in datos:
                longitud = input("Ingrese la longitud: ")
                s.sendall(longitud.encode())
                
                # Recibir la respuesta final del servidor
                datos = s.recv(1024).decode()
                print("Respuesta final del servidor:", datos)

if __name__ == "__main__":
    main()
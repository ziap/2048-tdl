
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <functional>

// Server related stuffs
void RunAgent(int client_socket) {
    printf("Accepted client socket %d\n", client_socket);
    char buffer[1 << 16] = {0};
    while (true) {
        buffer[0] = '\0';

        if (read(client_socket, buffer, sizeof(buffer)) < 0) break;
        buffer[sizeof(buffer) - 1] = '\0';

        char* get = strstr(buffer, "GET /move?board=");
        if (!get) break;
        char* board = strchr(get, '=') + 1;
        if (strlen(board) < 16) break;

        board_t layout = std::strtoull(board, NULL, 16);

        int dir = search(layout);

        std::string reply(
            "HTTP/1.1 200 OK\n"
            "Access-Control-Allow-Origin: *\n"
            "Content-Type: text/plain\n"
            "Content-Length: 1\n"
            "\n");
        reply += dir >= 0 ? "urdl"[dir] : 'g';

        if (write(client_socket, reply.c_str(), reply.size()) < 0) break;
    }
    close(client_socket);
    printf("Closed client socket %d\n", client_socket);
}

void RunServer(int server_port) {
    // Creating socket file descriptor
    int server_socket;
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    int enable = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enable,
                    sizeof(enable)) < 0) {
        perror("In setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(server_port);

    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    if (bind(server_socket, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_socket, 10) < 0) {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    puts("Server ready");
    while (true) {
        const int addrlen = sizeof(address);
        int client_socket;
        if ((client_socket = accept(server_socket, (struct sockaddr*)&address,
                                    (socklen_t*)&addrlen)) < 0) {
        perror("In accept");
        exit(EXIT_FAILURE);
        }
        new std::thread(std::bind(RunAgent, client_socket));
    }

    close(server_socket);
}
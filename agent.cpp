#include "board.h"
#include "search.h"
#include "seed.h"
#include <cstdlib>
#include <iostream>
#include <getopt.h>
#include <numeric>

#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <functional>
#include <thread>
#include <vector>
#include <cstring>
#include <cstdio>

#ifndef STRUCTURE
    #define STRUCTURE nw4x6
#endif

#ifndef FILE_NAME
    #define FILE_NAME "weights.bin"
#endif

bool showboard = false;
int games = 1, port = 0;
std::vector<double> speeds;
std::vector<double> scores;
int rate[16] = { 0 };

Search<STRUCTURE> search;

// Print statistics
void ShowStat(int n) {
    if (!showboard) std::cout << "\033[u";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "progress: " << n << '/' << games;
    std::cout << "\n\033[Kaverage score: " << std::accumulate(scores.begin(), scores.end(), 0.0) / double(n) << '\n';
    std::cout << "\033[Kaverage speed: " << std::accumulate(speeds.begin(), speeds.end(), 0.0) / (double)n << " moves per second\n";
    int accu = 0;
    for (int i = 15; i > 0; i--) {
        if (!rate[i]) continue;
        accu += rate[i];
        std::cout << "\033[K\t" << (1 << i) << '\t' << accu * 100.f / float(n) << "%\n";
    }
}

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

        float best = 0;
        int dir = -1;
        for (int i = 0; i < 4; i++) {
            float val = search(layout, i);
            if (val > best) {
                best = val;
                dir = i;
            }
        }

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

int main(int argc, char* argv[]) {
    search.Load(FILE_NAME);
    int c;
    while ((c = getopt(argc, argv, "d:i:sS:")) != -1) switch (c) {
    case 'd':
        search.min_depth = atoi(optarg);
        break;
    case 'i':
        games = atoi(optarg);
        break;
    case 's':
        showboard = true;
        break;
    case 'S':
        port = atoi(optarg);
        break;
    }
    if (port) {
        RunServer(port);
        return 0;
    }
    std::cout << "\x1B[2J\x1B[H";
    long long seed = RandomSeed();
    srand(seed);
    std::cout << "seed = " << seed << '\t' << "depth = " << search.min_depth << '\n';
    std::cout << "\033[s";

    // Play N games
    for (int n = 1; n <= games; n++) {
        int moves = 0;
        auto start = std::chrono::high_resolution_clock::now();
        board_t board = AddTile(AddTile(0));
        int score = 0;
        for (;;) {
            if (showboard) {
                std::cout << "\033[u";
                PrintBoard(board);
            }
            float best = 0;
            int dir = -1;
            for (int i = 0; i < 4; i++) {
                float val = search(board, i);
                if (val > best) {
                    best = val;
                    dir = i;
                }
            }
            if (dir < 0) break;
            moves++;
            score += move.Score(board, dir);
            board = AddTile(move(board, dir));
        }
        auto end = std::chrono::high_resolution_clock::now();
        speeds.push_back((double)moves * 1e9 / (double)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
        scores.push_back(score);
        rate[MaxRank(board)]++;
        ShowStat(n);
    }
    std::cout << '\n';
    return 0;
}

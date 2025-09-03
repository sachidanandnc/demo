#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ncurses.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Change IP to your server's IP if on LAN
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed\n");
        return -1;
    }

    // Setup ncurses for non-blocking input
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    printw("Use W/A/S/D to send inputs. Press Q to quit.\n");
    refresh();

    while (1) {
        int ch = getch();
        if (ch != ERR) {
            char key[2] = {0};
            switch (ch) {
                case 'w': case 'W': key[0] = 'W'; break;
                case 'a': case 'A': key[0] = 'A'; break;
                case 's': case 'S': key[0] = 'S'; break;
                case 'd': case 'D': key[0] = 'D'; break;
                case 'q': case 'Q': endwin(); close(sock); return 0;
            }
            if (key[0] != 0) {
                send(sock, key, 1, 0);
            }
        }
        usleep(50000); // 50ms
    }

    endwin();
    close(sock);
    return 0;
}


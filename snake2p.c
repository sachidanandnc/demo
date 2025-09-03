#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define DELAY 120000  // microseconds per frame

typedef struct Snake {
    int x[100], y[100];
    int length;
    int dir;   // 0=UP, 1=RIGHT, 2=DOWN, 3=LEFT
    char symbol;
} Snake;

int max_x, max_y;
int food_x, food_y;

void place_food() {
    food_x = rand() % (max_x - 2) + 1;
    food_y = rand() % (max_y - 2) + 1;
}

void draw_snake(Snake *s) {
    for (int i = 0; i < s->length; i++) {
        mvprintw(s->y[i], s->x[i], "%c", s->symbol);
    }
}

void move_snake(Snake *s) {
    for (int i = s->length - 1; i > 0; i--) {
        s->x[i] = s->x[i - 1];
        s->y[i] = s->y[i - 1];
    }
    switch (s->dir) {
        case 0: s->y[0]--; break;
        case 1: s->x[0]++; break;
        case 2: s->y[0]++; break;
        case 3: s->x[0]--; break;
    }
}

int check_collision(Snake *s) {
    // Wall collision
    if (s->x[0] <= 0 || s->x[0] >= max_x - 1 || s->y[0] <= 0 || s->y[0] >= max_y - 1)
        return 1;

    // Self collision
    for (int i = 1; i < s->length; i++) {
        if (s->x[0] == s->x[i] && s->y[0] == s->y[i])
            return 1;
    }
    return 0;
}

int check_snake_collision(Snake *a, Snake *b) {
    // Check if head of A hits body of B
    for (int i = 0; i < b->length; i++) {
        if (a->x[0] == b->x[i] && a->y[0] == b->y[i])
            return 1;
    }
    return 0;
}

int main() {
    srand(time(NULL));

    Snake p1 = {{10,9,8}, {10,10,10}, 3, 1, 'O'}; // Player 1
    Snake p2 = {{30,31,32}, {15,15,15}, 3, 3, 'X'}; // Player 2

    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    getmaxyx(stdscr, max_y, max_x);

    place_food();

    while (1) {
        clear();

        // Borders
        for (int i = 0; i < max_x; i++) {
            mvprintw(0, i, "#");
            mvprintw(max_y-1, i, "#");
        }
        for (int i = 0; i < max_y; i++) {
            mvprintw(i, 0, "#");
            mvprintw(i, max_x-1, "#");
        }

        mvprintw(food_y, food_x, "*");
        draw_snake(&p1);
        draw_snake(&p2);

        refresh();

        int ch = getch();
        switch (ch) {
            // Player 1 controls (arrows)
            case KEY_UP:    if (p1.dir != 2) p1.dir = 0; break;
            case KEY_RIGHT: if (p1.dir != 3) p1.dir = 1; break;
            case KEY_DOWN:  if (p1.dir != 0) p1.dir = 2; break;
            case KEY_LEFT:  if (p1.dir != 1) p1.dir = 3; break;

            // Player 2 controls (WASD)
            case 'w': case 'W': if (p2.dir != 2) p2.dir = 0; break;
            case 'd': case 'D': if (p2.dir != 3) p2.dir = 1; break;
            case 's': case 'S': if (p2.dir != 0) p2.dir = 2; break;
            case 'a': case 'A': if (p2.dir != 1) p2.dir = 3; break;

            case 'q': endwin(); return 0;
        }

        move_snake(&p1);
        move_snake(&p2);

        // Check food
        if (p1.x[0] == food_x && p1.y[0] == food_y) {
            p1.length++;
            place_food();
        }
        if (p2.x[0] == food_x && p2.y[0] == food_y) {
            p2.length++;
            place_food();
        }

        // Collisions
        if (check_collision(&p1) || check_snake_collision(&p1, &p2)) {
            mvprintw(max_y/2, max_x/2 - 6, "Player 2 Wins!");
            refresh();
            sleep(2);
            break;
        }
        if (check_collision(&p2) || check_snake_collision(&p2, &p1)) {
            mvprintw(max_y/2, max_x/2 - 6, "Player 1 Wins!");
            refresh();
            sleep(2);
            break;
        }

        usleep(DELAY);
    }

    endwin();
    return 0;
}


#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define DELAY 120000  // microseconds between updates

typedef struct Snake {
    int x[100], y[100];   // snake body positions
    int length;
    int dir;              // 0=UP, 1=RIGHT, 2=DOWN, 3=LEFT
} Snake;

int max_x, max_y;  // screen size
int food_x, food_y;

void place_food() {
    food_x = rand() % (max_x - 2) + 1;
    food_y = rand() % (max_y - 2) + 1;
}

void draw_snake(Snake *s) {
    for (int i = 0; i < s->length; i++) {
        mvprintw(s->y[i], s->x[i], "O");
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
    // wall collision
    if (s->x[0] <= 0 || s->x[0] >= max_x-1 || s->y[0] <= 0 || s->y[0] >= max_y-1)
        return 1;
    // self collision
    for (int i = 1; i < s->length; i++) {
        if (s->x[0] == s->x[i] && s->y[0] == s->y[i])
            return 1;
    }
    return 0;
}

int main() {
    Snake snake;
    snake.length = 3;
    snake.x[0] = 10; snake.y[0] = 10;
    snake.x[1] = 9;  snake.y[1] = 10;
    snake.x[2] = 8;  snake.y[2] = 10;
    snake.dir = 1; // moving right

    srand(time(NULL));

    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE); // non-blocking input
    getmaxyx(stdscr, max_y, max_x);

    place_food();

    while (1) {
        clear();

        // draw borders
        for (int i = 0; i < max_x; i++) {
            mvprintw(0, i, "#");
            mvprintw(max_y-1, i, "#");
        }
        for (int i = 0; i < max_y; i++) {
            mvprintw(i, 0, "#");
            mvprintw(i, max_x-1, "#");
        }

        mvprintw(food_y, food_x, "*");  // draw food
        draw_snake(&snake);

        refresh();

        int ch = getch();
        switch (ch) {
            case KEY_UP:    if (snake.dir != 2) snake.dir = 0; break;
            case KEY_RIGHT: if (snake.dir != 3) snake.dir = 1; break;
            case KEY_DOWN:  if (snake.dir != 0) snake.dir = 2; break;
            case KEY_LEFT:  if (snake.dir != 1) snake.dir = 3; break;
            case 'q': endwin(); return 0;  // quit
        }

        move_snake(&snake);

        // food eaten
        if (snake.x[0] == food_x && snake.y[0] == food_y) {
            snake.length++;
            place_food();
        }

        if (check_collision(&snake)) {
            mvprintw(max_y/2, max_x/2 - 5, "GAME OVER!");
            refresh();
            sleep(2);
            break;
        }

        usleep(DELAY);
    }

    endwin();
    return 0;
}


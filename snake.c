#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <ncurses.h>

#define WIDTH 50
#define HEIGHT 20
#define INITIAL_LENGTH 5
#define SPEED 150000

typedef struct {
    int x;
    int y;
} Point;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef struct {
    Point head;
    Point body[WIDTH * HEIGHT];
    Direction direction;
    int length;
} Snake;

typedef struct {
    Point position;
    bool active;
} Food;

void setup();
void draw();
void input();
void logic();
void spawn_food();
bool collision(Point p);
void game_over();
void cleanup();

Snake snake;
Food food;
bool running = true;

int main() {
    setup();

    while (running) {
        draw();
        input();
        logic();
        usleep(SPEED);
    }

    cleanup();
    return 0;
}

void setup() {
    initscr();
    clear();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(0);

    snake.head.x = WIDTH / 2;
    snake.head.y = HEIGHT / 2;
    snake.direction = RIGHT;
    snake.length = INITIAL_LENGTH;

    for (int i = 0; i < snake.length; i++) {
        snake.body[i].x = snake.head.x - i;
        snake.body[i].y = snake.head.y;
    }

    spawn_food();
}

void draw() {
    clear();

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) {
                mvprintw(i, j, "#");
            } else if (i == food.position.y && j == food.position.x && food.active) {
                mvprintw(i, j, "F");
            } else {
                bool is_snake = false;

                for (int k = 0; k < snake.length; k++) {
                    if (snake.body[k].x == j && snake.body[k].y == i) {
                        mvprintw(i, j, "O");
                        is_snake = true;
                        break;
                    }
                }

                if (!is_snake) {
                    mvprintw(i, j, " ");
                }
            }
        }
    }

    refresh();
}

void input() {
    int key = getch();

    switch (key) {
        case KEY_UP:
            if (snake.direction != DOWN) {
                snake.direction = UP;
            }
            break;
        case KEY_DOWN:
            if (snake.direction != UP) {
                snake.direction = DOWN;
            }
            break;
        case KEY_LEFT:
            if (snake.direction != RIGHT) {
                snake.direction = LEFT;
            }
            break;
        case KEY_RIGHT:
            if (snake.direction != LEFT) {
                snake.direction = RIGHT;
            }
            break;
        case 'q':
            running = false;
            break;
    }
}

void logic() {
    Point prev = snake.body[0];
    Point temp;

    snake.body[0] = snake.head;

    for (int i = 1; i < snake.length; i++) {
        temp = snake.body[i];
        snake.body[i] = prev;
        prev = temp;
    }

    switch (snake.direction) {
        case UP:
            snake.head.y--;
            break;
        case DOWN:
            snake.head.y++;
            break;
        case LEFT:
            snake.head.x--;
            break;
        case RIGHT:
            snake.head.x++;
            break;
    }

    if (collision(snake.head)) {
        game_over();
        return;
    }

    if (snake.head.x == food.position.x && snake.head.y == food.position.y && food.active) {
        snake.length++;
        spawn_food();
    }
}

void spawn_food() {
    srand(time(NULL));
    int x, y;

    do {
        x = rand() % (WIDTH - 2) + 1;
        y = rand() % (HEIGHT - 2) + 1;
    } while (collision((Point){x, y}));

    food.position.x = x;
    food.position.y = y;
    food.active = true;
}

bool collision(Point p) {
    if (p.x <= 0 || p.x >= WIDTH - 1 || p.y <= 0 || p.y >= HEIGHT - 1) {
        return true;
    }

    for (int i = 0; i < snake.length; i++) {
        if (snake.body[i].x == p.x && snake.body[i].y == p.y) {
            return true;
        }
    }

    return false;
}

void game_over() {
    mvprintw(HEIGHT / 2, WIDTH / 2 - 5, "Game Over!");
    mvprintw(HEIGHT / 2 + 1, WIDTH / 2 - 8, "Press any key to quit");
    refresh();
    getch();
    running = false;
}

void cleanup() {
    endwin();
}


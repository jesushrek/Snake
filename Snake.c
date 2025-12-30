#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#define WIDTH 20
#define HEIGHT 10

typedef enum
{ 
    Up = 0,
    Down,
    Left,
    Right,
    None,
}Direction;

typedef struct 
{ 
    int x;
    int y;
}Point;

/*

typedef enum 
{ 
    WithBody = 0,
    WithFruit,
    WithNone,
}CollisonType;
*/


typedef struct
{ 
    Point body[WIDTH * HEIGHT];
    int size;
    Direction d;
    bool isAlive;
}Sstatus;

void printGrid(int* grid)
{ 
    for(int i = 0; i < HEIGHT; ++i)
    { 
        for(int j = 0; j < WIDTH; ++j)
        { 
            if(grid[i*WIDTH + j] == 1)
                printf("#"); 
            else if(grid[i*WIDTH +j] == 3)
                printf("o");
            else
                printf(" "); 
        }
        printf("\n");
    }
}

/*
CollisonType checkColison(Sstatus* snake, Point* fruit)
{ 
    if(snake->body[0].x == fruit->x && snake->body[0].y == fruit->y)
        return WithFruit;
    for(int i = 1; i < snake->size; ++i)
    {
        if(snake->body[0].x == snake->body[i].x && snake->body[0].y == snake->body[i].y)
            return WithBody;
    }
    return WithNone;
}
*/

Sstatus* updateSnake(Sstatus* snake, Point* fruit)
{   
    Point toMove = snake->body[0];

    if(snake->d == None) return snake;

    switch(snake->d)
    { 
        case Up:
            toMove.y = (snake->body[0].y - 1 + HEIGHT) % HEIGHT;
            break;

        case Left:
            toMove.x = (snake->body[0].x - 1 + WIDTH) % WIDTH;
            break;

        case Right:
            toMove.x = (snake->body[0].x + 1) % WIDTH;
            break;

        case Down:
            toMove.y = (snake->body[0].y + 1 ) % HEIGHT;
            break;

        default:
            break;
    };

    // check for tail collison
    for(int i = 1; i < snake->size; ++i)
    { 
        if(toMove.x == snake->body[i].x && toMove.y == snake->body[i].y)
        { 
            snake->isAlive = false;
            return snake;
        }
    }

    if(toMove.x == fruit->x && toMove.y == fruit->y)
    {
        bool onSnake = false;
        do 
        { 
        ++snake->size;
            onSnake = false;
            fruit->x = rand() % WIDTH;
            fruit->y = rand() % HEIGHT;

            for(int i = 0; i < (snake->size); ++i)
            { 
                if(fruit->x == snake->body[i].x && fruit->y == snake->body[i].y)
                {
                    onSnake = true;
                    break;
                }
            }

        } while(onSnake);
    }

    for(int i = (snake->size-1); i > 0; --i)
    { 
        snake->body[i] = snake->body[i-1];
    }

    snake->body[0] = toMove;
    return snake;
    /*
    for(int i = (snake->size-1); i > 0; --i)
    { 
        snake->body[i].x = snake->body[i-1].x;
        snake->body[i].y = snake->body[i-1].y;
    }
    switch(checkColison(snake, fruit)) 
    { 
        case WithBody:
            snake->isAlive = false;
            break;

        case WithFruit:
            int i = 0;
            ++snake->size;

            bool onSnake = false;
            do 
            { 
                onSnake = false;
                fruit->x = rand() % WIDTH;
                fruit->y = rand() % HEIGHT;

                for(int i = 0; i < snake->size; ++i)
                { 
                    if(fruit->x == snake->body[i].x && fruit->y == snake->body[i].y)
                    {
                        onSnake = true;
                        break;
                    }
                }

            } while(onSnake);

            break;
        default:
            break;

    };
    */
}

void updateGrid(int* grid, Sstatus* snake, Point* fruit)
{ 
    memset(grid, 0, sizeof(int) * HEIGHT * WIDTH);
    grid[fruit->y * WIDTH + fruit->x] = 3;
    for(int i = 0; i < snake->size; ++i)
    {
        grid[snake->body[i].y * WIDTH + snake->body[i].x] = 1;
    }
}

Direction getDirection() 
{ 
    char c;
    read(STDIN_FILENO, &c, sizeof(c));
    switch(c)
    { 
        case 'k': 
            return Up;
        case 'h': 
            return Left;
        case 'j': 
            return Down;
        case 'l': 
            return Right;
        default: 
            return None;
    }
    return None;
}

void setTerminalMode(bool enable)
{ 
    static struct termios oldt, newt;
    if(enable) 
    { 
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    }
    else 
    { 
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}

int main() 
{ 

    srand(time(0));
    int* grid = calloc(HEIGHT*WIDTH, sizeof(int));

    Point fruit = { rand() % WIDTH, rand() % HEIGHT };

    Sstatus snake = { 
        .body[0] = { WIDTH/2, HEIGHT/2},
        .size = 1,
        .d = None,
        .isAlive = true,
    };

    int flags = fcntl(STDIN_FILENO, F_GETFL);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    setTerminalMode(true);

    bool isFirst = true;
    while(snake.isAlive)
    { 
        printf("\033[H");
        Direction newDirection = getDirection();

        if(newDirection != None)
        { 
            if(snake.d == None)
            { 
                snake.d = newDirection;
            }
            if((snake.d == Up && newDirection != Down) 
                    || (snake.d == Down && newDirection != Up)
                    || (snake.d == Left && newDirection != Right)
                    || (snake.d == Right && newDirection != Left)
              )
            { 
                snake.d = newDirection;
            }
        }
        updateSnake(&snake, &fruit);
        updateGrid(grid, &snake, &fruit);
        printGrid(grid);
        usleep(150 * 1000);
        isFirst = false;
    }

    printf("Game over!\n");
    // fix the terminal and free the grid array
    fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
    setTerminalMode(false);
    free(grid);
    return 0;
}
JJ

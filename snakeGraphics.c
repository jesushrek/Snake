#include <raylib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define WIDTH 50
#define HEIGHT 30
#define CELL 15 

typedef enum 
{ 
    Up, 
    Down,
    Left,
    Right,
    maxDirection,

}Direction;

typedef struct { int x, y; }Point;

typedef struct 
{ 
    Point body[WIDTH * HEIGHT];
    int size;
    bool isAlive;
    Direction d;

}Snake;

typedef struct 
{
    Point point;
}Fruit; 


Point directionToPoint(Direction newDirection, Point current)
{ 
    Point new = current;
    switch(newDirection)
    { 
        case Up:
            new.y = (current.y + HEIGHT - 1) % HEIGHT;
            break;
        case Down:
            new.y = (current.y + 1) % HEIGHT;
            break;
        case Left:
            new.x = (current.x + WIDTH - 1) % WIDTH;
            break;
        case Right:
            new.x = (current.x + 1) % WIDTH;
            break;
        case maxDirection:
            break;
    }
    return new;
}

void drawGame(Snake* snake, Fruit* fruit)
{ 
    //DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color);

    int screenHeight = HEIGHT * CELL;
    int screenWidth = WIDTH * CELL;

    for(int i = 0; i < WIDTH; ++i)
        DrawLine( i * CELL, 0, i * CELL, screenHeight, LIGHTGRAY);

    for(int i = 0; i < HEIGHT; ++i)
        DrawLine(0, i * CELL, screenWidth, i * CELL, LIGHTGRAY);

    Point head = snake->body[0]; 

    DrawRectangle(head.x * CELL, head.y * CELL, CELL, CELL, SKYBLUE);
    
    float eyeSize = CELL * 0.4f;
    float margin = CELL * 0.1f;
    Color eyeColor = BLACK;

    if(snake->d == Right)
    { 
        DrawRectangle(head.x * CELL, head.y * CELL, eyeSize, eyeSize, eyeColor);
        DrawRectangle(head.x * CELL, head.y * CELL + eyeSize + margin, eyeSize, eyeSize, eyeColor);
    }
    if(snake->d == Up)
    { 
        DrawRectangle(head.x * CELL, head.y * CELL + eyeSize + margin, eyeSize, eyeSize, eyeColor);
        DrawRectangle(head.x * CELL + eyeSize + margin, head.y * CELL + eyeSize + margin, eyeSize, eyeSize, eyeColor);
    }
    if(snake->d == Down)
    { 
        DrawRectangle(head.x * CELL, head.y * CELL, eyeSize, eyeSize, eyeColor);
        DrawRectangle(head.x * CELL + eyeSize + margin, head.y * CELL, eyeSize, eyeSize, eyeColor);
    }
    if(snake->d == Left)
    { 
        DrawRectangle(head.x * CELL + eyeSize + margin, head.y * CELL, eyeSize, eyeSize, eyeColor);
        DrawRectangle(head.x * CELL + eyeSize + margin, head.y * CELL + eyeSize + margin, eyeSize, eyeSize, eyeColor);
    }

    for(int i = 1; i < snake->size; ++i)
        DrawRectangle(snake->body[i].x * CELL, snake->body[i].y * CELL, CELL, CELL, BLUE);

    DrawRectangle(fruit->point.x * CELL, fruit->point.y * CELL, CELL, CELL, RED);
}

bool updateSnake(Snake* snake, Fruit* fruit)
{
    Point newHead = directionToPoint(snake->d, snake->body[0]);

    if(newHead.x == fruit->point.x && newHead.y == fruit->point.y)
        ++snake->size;

    for(int i = snake->size - 1; i > 0; --i)
        snake->body[i] = snake->body[i-1];

    snake->body[0] = newHead;
    for(int i = 1; i < snake->size; ++i)
    { 
        if(snake->body[0].x == snake->body[i].x && snake->body[0].y == snake->body[i].y)
            return false;
    }

    if(newHead.x == fruit->point.x && newHead.y == fruit->point.y)
    { 
        bool onSnake = false;
        do
        { 
            onSnake = false;
            fruit->point.x = rand() % WIDTH;
            fruit->point.y = rand() % HEIGHT;

            for(int i = 0; i < snake->size; ++i)
            { 
                if(fruit->point.x == snake->body[i].x && fruit->point.y == snake->body[i].y)
                { 
                    onSnake = true;
                    break;
                }
            }

        }while(onSnake);
    }

    return true;
}

Direction keyToDirection(Snake* snake)
{ 
    if(IsKeyDown(KEY_J))
        return Down;
    if(IsKeyDown(KEY_K))
        return Up;
    if(IsKeyDown(KEY_L))
        return Right;
    if(IsKeyDown(KEY_H))
        return Left;

    return snake->d;
}


int main() 
{ 
    int screenWidth = WIDTH * CELL;
    int screenHeight = HEIGHT * CELL;

    int frameCounter = 0;
    int speedLimit = 15;

    Snake snake = {
        .body[0] = { HEIGHT / 2, WIDTH / 2 },
        .size = 1,
        .isAlive = true,
        .d = maxDirection,
    };

    Fruit fruit = { 
        .point = { rand() % WIDTH, rand() % HEIGHT },
    };

    InitWindow(screenWidth, screenHeight, "Snake");
    SetTargetFPS(144);
    ClearBackground(WHITE);

    while(!WindowShouldClose())
    { 
        Direction newDirection = maxDirection;
        newDirection = keyToDirection(&snake);

        if(snake.size != 0 && snake.d != maxDirection)
        { 
            if(snake.d == Left && newDirection == Right 
                    || snake.d == Right && newDirection == Left
                    || snake.d == Up && newDirection == Down
                    || snake.d == Down && newDirection == Up)
                newDirection = snake.d;
        }

        ++frameCounter;
        if(frameCounter == speedLimit)
        { 
            if(!updateSnake(&snake, &fruit)) break;
            frameCounter = 0;
        }

        snake.d = newDirection;
        BeginDrawing();
        ClearBackground(WHITE);
        drawGame(&snake, &fruit);
        EndDrawing();
    }
    CloseWindow();

    FILE* file = fopen("/tmp/score.txt", "wb");
    fprintf(file, "%d", snake.size);
    fclose(file);

    return 0;
}

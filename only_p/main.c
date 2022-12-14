#include "stdio.h"
#include "stdlib.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL2_gfxPrimitives.h"
#include "time.h"

#include "formulas.h"
#include "wall.h"
#include "robot.h"
#include <unistd.h>

int done = 0;


int main(int argc, char *argv[]) {
    SDL_Window *window;
    SDL_Renderer *renderer;

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        return 1;
    }

    window = SDL_CreateWindow("Robot Maze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    window = SDL_CreateWindow("Robot Maze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, -1, 0);

    struct Robot robot;
    struct Wall_collection *head = NULL;
    int front_centre_sensor, left_sensor, right_sensor=0;
    clock_t start_time, end_time;
    int msec;
    int crashed = 0;
    // Map with djead ends, tight sections
    insertAndSetFirstWall(&head, 1,  150, OVERALL_WINDOW_HEIGHT/2 - 50, 10, OVERALL_WINDOW_HEIGHT/2);
    insertAndSetFirstWall(&head, 1,  100, OVERALL_WINDOW_HEIGHT/2, 10, OVERALL_WINDOW_HEIGHT/2-50);
    insertAndSetFirstWall(&head, 2,  50, OVERALL_WINDOW_HEIGHT/2-55, OVERALL_WINDOW_WIDTH/6, 10);
    insertAndSetFirstWall(&head, 2,  0, OVERALL_WINDOW_HEIGHT/2, OVERALL_WINDOW_WIDTH/6, 400);
    insertAndSetFirstWall(&head, 1,  45, 55, 10, OVERALL_WINDOW_HEIGHT/2-100); 
    insertAndSetFirstWall(&head, 2,  45, 45, OVERALL_WINDOW_WIDTH/6, 10);
    insertAndSetFirstWall(&head, 2,  0, OVERALL_WINDOW_HEIGHT-50, OVERALL_WINDOW_WIDTH, 60); 
    insertAndSetFirstWall(&head, 3,  0, 0, OVERALL_WINDOW_WIDTH, 10); 
    insertAndSetFirstWall(&head, 4,  OVERALL_WINDOW_WIDTH/2, OVERALL_WINDOW_HEIGHT-115, OVERALL_WINDOW_WIDTH, 10); 
    insertAndSetFirstWall(&head, 5,  OVERALL_WINDOW_WIDTH-50, 70, 10, OVERALL_WINDOW_HEIGHT);
    insertAndSetFirstWall(&head, 5,  OVERALL_WINDOW_WIDTH-50, 70, 100, 700);
    insertAndSetFirstWall(&head, 5,  0, 0, 10, OVERALL_WINDOW_HEIGHT);
    insertAndSetFirstWall(&head, 6,  OVERALL_WINDOW_WIDTH/2, OVERALL_WINDOW_HEIGHT-185, 210, 10);
    insertAndSetFirstWall(&head, 6,  OVERALL_WINDOW_WIDTH-180, 60, 10, 240);
    insertAndSetFirstWall(&head, 7,  OVERALL_WINDOW_WIDTH-115, 0, 10, OVERALL_WINDOW_HEIGHT/2+65);


    // int i, a, b, c, d, e, f, g, h, k, l, m;
    // double j;
    // a = 350;
    // b = 40;
    // c = 10;
    // d = 3;
    // e = 90;
    // f = 200;
    // g = b;
    // h = c;
    // k = d;
    // l = e;
    // m = 500;
    // int name_index = 0;
    // for (i = 0; i < m; i++){
    //     j = i;
    //     insertAndSetFirstWall(&head, name_index++,
    //                             a + b*sin(c*j * M_PI/180),
    //                             (i * d)+e,
    //                             10, 10);
    //     insertAndSetFirstWall(&head, name_index++,
    //                             f + g*sin(h*j * M_PI/180),
    //                             (i * k)+l,
    //                             10, 10);
    // }
    // // int i, a, b, c, d, e, f, g, h, k, l, m;
    // double j;
    // a = 300;
    // b = 30;
    // c = 10;
    // d = 3;
    // e = 100;
    // f = 200;
    // g = b;
    // h = c;
    // k = d;
    // l = e;
    // m = 50;
    // for (i = 0; i < m; i++){
    //     j = i;
    //     insertAndSetFirstWall(&head, name_index++,
    //                           a + b*sin(c*j * M_PI/180),
    //                           (i * d)+e,
    //                           10, 10);
    //     insertAndSetFirstWall(&head, name_index++,
    //                           f + g*sin(h*j * M_PI/180),
    //                           (i * k)+l,
    //                           10, 10);
    // }

    // float aa, bb;
    // a = 500;
    // aa = 0.5;
    // bb = 1;
    // c = 50;
    // d = 1;
    // e = a+100;
    // f = c;
    // m = 200;
    // for (i = 0; i < m; i++){

    //     insertAndSetFirstWall(&head, name_index++,  a - i*aa , c + i*bb, 10, 10);
    //     insertAndSetFirstWall(&head, name_index++,  e - i*aa , f + i*bb, 10, 10);
    // }
// SETUP MAZE
    // You can create your own maze here. line of code is adding a wall.
    // You describe position of top left corner of wall (x, y), then width and height going down/to right
    // Relative positions are used (OVERALL_WINDOW_WIDTH and OVERALL_WINDOW_HEIGHT)
    // But you can use absolute positions. 10 is used as the width, but you can change this.
    // insertAndSetFirstWall(&head, 1,  OVERALL_WINDOW_WIDTH/2, OVERALL_WINDOW_HEIGHT/2, 10, OVERALL_WINDOW_HEIGHT/2);
    // insertAndSetFirstWall(&head, 2,  OVERALL_WINDOW_WIDTH/2-100, OVERALL_WINDOW_HEIGHT/2+100, 10, OVERALL_WINDOW_HEIGHT/2-100);
    // insertAndSetFirstWall(&head, 3,  OVERALL_WINDOW_WIDTH/2-250, OVERALL_WINDOW_HEIGHT/2+100, 150, 10);
    // insertAndSetFirstWall(&head, 4,  OVERALL_WINDOW_WIDTH/2-150, OVERALL_WINDOW_HEIGHT/2, 150, 10);
    // insertAndSetFirstWall(&head, 5,  OVERALL_WINDOW_WIDTH/2-250, OVERALL_WINDOW_HEIGHT/2-200, 10, 300);
    // insertAndSetFirstWall(&head, 6,  OVERALL_WINDOW_WIDTH/2-150, OVERALL_WINDOW_HEIGHT/2-100, 10, 100);
    // insertAndSetFirstWall(&head, 7,  OVERALL_WINDOW_WIDTH/2-250, OVERALL_WINDOW_HEIGHT/2-200, 450, 10);

    // insertAndSetFirstWall(&head, 8,  OVERALL_WINDOW_WIDTH/2-150, OVERALL_WINDOW_HEIGHT/2-100, 450, 10);

    // insertAndSetFirstWall(&head, 9,  OVERALL_WINDOW_WIDTH/2+200, OVERALL_WINDOW_HEIGHT/2-200, 10, 300);

    // insertAndSetFirstWall(&head, 10,  OVERALL_WINDOW_WIDTH/2+100, OVERALL_WINDOW_HEIGHT/2-100, 10, 300);

    // insertAndSetFirstWall(&head, 11,  OVERALL_WINDOW_WIDTH/2+100, OVERALL_WINDOW_HEIGHT/2+200, OVERALL_WINDOW_WIDTH/2-100, 10);
    // insertAndSetFirstWall(&head, 12,  OVERALL_WINDOW_WIDTH/2+200, OVERALL_WINDOW_HEIGHT/2+100, OVERALL_WINDOW_WIDTH/2-100, 10);

    setup_robot(&robot);
    updateAllWalls(head, renderer);

    SDL_Event event;
    while(!done){
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderClear(renderer);

        //Move robot based on user input commands/auto commands
        if (robot.auto_mode == 1)
            robotAutoMotorMove(&robot, left_sensor, right_sensor, front_centre_sensor);
        robotMotorMove(&robot, crashed);

        //Check if robot reaches endpoint. and check sensor values
        if (checkRobotReachedEnd(&robot, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT/2+100, 10, 100)){
            end_time = clock();
            msec = (end_time-start_time) * 10 / sysconf(_SC_CLK_TCK);
            robotSuccess(&robot, msec);
        }
        else if(crashed == 1 || checkRobotHitWalls(&robot, head)){
            robotCrash(&robot);
            crashed = 1;
        }
        //Otherwise compute sensor information
        else {
            front_centre_sensor = checkRobotSensorFrontCentreAllWalls(&robot, head);
            if (front_centre_sensor>0)
                printf("Getting close on the centre. Score = %d\n", front_centre_sensor);

            left_sensor = checkRobotSensorLeftAllWalls(&robot, head);
            if (left_sensor>0)
                printf("Getting close on the left. Score = %d\n", left_sensor);

            right_sensor = checkRobotSensorRightAllWalls(&robot, head);
            if (right_sensor>0)
                printf("Getting close on the right. Score = %d\n", right_sensor);
        }
        robotUpdate(renderer, &robot);
        updateAllWalls(head, renderer);

        // Check for user input
        SDL_RenderPresent(renderer);
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                done = 1;
            }
            const Uint8 *state = SDL_GetKeyboardState(NULL);
            if(state[SDL_SCANCODE_UP] && robot.direction != DOWN){
                robot.direction = UP;
            }
            if(state[SDL_SCANCODE_DOWN] && robot.direction != UP){
                robot.direction = DOWN;
            }
            if(state[SDL_SCANCODE_LEFT] && robot.direction != RIGHT){
                robot.direction = LEFT;
            }
            if(state[SDL_SCANCODE_RIGHT] && robot.direction != LEFT){
                robot.direction = RIGHT;
            }
            if(state[SDL_SCANCODE_SPACE]){
                setup_robot(&robot);
            }
            if(state[SDL_SCANCODE_RETURN]){
                robot.auto_mode = 1;
                start_time = clock();
            }
        }

        SDL_Delay(120);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("DEAD\n");
}

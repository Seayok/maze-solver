#include "robot.h"
#include <stdbool.h>

void setup_robot(struct Robot* robot) {
    robot->x = OVERALL_WINDOW_WIDTH / 2 - 50;
    robot->y = OVERALL_WINDOW_HEIGHT - 50;
    robot->true_x = OVERALL_WINDOW_WIDTH / 2 - 50;
    robot->true_y = OVERALL_WINDOW_HEIGHT - 50;
    // robot->true_x = 117;
    // robot->true_y = OVERALL_WINDOW_HEIGHT-100;
    robot->width = ROBOT_WIDTH;
    robot->height = ROBOT_HEIGHT;
    robot->direction = 0;
    robot->angle = 0;
    robot->currentSpeed = 0;
    robot->crashed = 0;
    robot->auto_mode = 0;

    robot->desired = 3;
    robot->total_dir_change = 0;
    robot->found_wall = 0;
    robot->switch_hand = 0;
    robot->ki = 0.5;
    robot->kd = 1;//2.358
    robot->kp = 15;//7
    robot->kiTotal = 0;
    robot->total_err = 0;
    robot->prior_error = 0;


    printf("Press arrow keys to move manually, or enter to move automatically\n\n");
}
int robot_off_screen(struct Robot* robot) {
    if (robot->x < 0 || robot->y < 0) {
        return 0;
    }
    if (robot->x > OVERALL_WINDOW_WIDTH || robot->y > OVERALL_WINDOW_HEIGHT) {
        return 0;
    }
    return 1;
}

int checkRobotHitWall(struct Robot* robot, struct Wall* wall) {

    int overlap = checkOverlap(robot->x, robot->width, robot->y, robot->height,
        wall->x, wall->width, wall->y, wall->height);

    return overlap;
}

int checkRobotHitWalls(struct Robot* robot, struct Wall_collection* head) {
    struct Wall_collection* ptr = head;
    int hit = 0;

    while (ptr != NULL) {
        hit = (hit || checkRobotHitWall(robot, &ptr->wall));
        ptr = ptr->next;
    }
    return hit;

}

int checkRobotReachedEnd(struct Robot* robot, int x, int y, int width, int height) {

    int overlap = checkOverlap(robot->x, robot->width, robot->y, robot->height,
        x, width, y, height);

    return overlap;
}

void robotCrash(struct Robot* robot) {
    robot->currentSpeed = 0;
    if (!robot->crashed)
        printf("Ouchies!!!!!\n\nPress space to start again\n");
    robot->crashed = 1;
}

void robotSuccess(struct Robot* robot, int msec) {
    robot->currentSpeed = 0;
    if (!robot->crashed) {
        printf("Success!!!!!\n\n");
        printf("Time taken %d seconds %d milliseconds \n", msec / 1000, msec % 1000);
        printf("Press space to start again\n");
    }
    robot->crashed = 1;
}

int checkRobotSensor(int x, int y, int sensorSensitivityLength, struct Wall* wall) {
    //viewing_region of sensor is a square of 2 pixels * chosen length of sensitivity
    int overlap = checkOverlap(x, 2, y, sensorSensitivityLength,
        wall->x, wall->width, wall->y, wall->height);

    return overlap;
}

int checkRobotSensorFrontCentreAllWalls(struct Robot* robot, struct Wall_collection* head) {
    struct Wall_collection* ptr, * head_store;
    int i;
    double xDir, yDir;
    int robotCentreX, robotCentreY, xTL, yTL;
    int score, hit;

    int sensorSensitivityLength = floor(SENSOR_VISION / 5);

    head_store = head;
    robotCentreX = robot->x + ROBOT_WIDTH / 2;
    robotCentreY = robot->y + ROBOT_HEIGHT / 2;
    score = 0;

    for (i = 0; i < 5; i++)
    {
        ptr = head_store;
        //xDir = round(robotCentreX+(ROBOT_WIDTH/2-2)*cos((robot->angle)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*sin((robot->angle)*PI/180));
        //yDir = round(robotCentreY+(ROBOT_WIDTH/2-2)*sin((robot->angle)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*cos((robot->angle)*PI/180));
        xDir = round(robotCentreX - (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensorSensitivityLength * i) * sin((robot->angle) * PI / 180));
        yDir = round(robotCentreY + (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensorSensitivityLength * i) * cos((robot->angle) * PI / 180));
        xTL = (int)xDir;
        yTL = (int)yDir;
        hit = 0;

        while (ptr != NULL) {
            hit = (hit || checkRobotSensor(xTL, yTL, sensorSensitivityLength, &ptr->wall));
            ptr = ptr->next;
        }
        if (hit)
            score = i;
    }
    return score;
}

int checkRobotSensorLeftAllWalls(struct Robot* robot, struct Wall_collection* head) {
    struct Wall_collection* ptr, * head_store;
    int i;
    double xDir, yDir;
    int robotCentreX, robotCentreY, xTL, yTL;
    int score, hit;
    int sensorSensitivityLength;

    head_store = head;
    robotCentreX = robot->x + ROBOT_WIDTH / 2;
    robotCentreY = robot->y + ROBOT_HEIGHT / 2;
    score = 0;
    sensorSensitivityLength = floor(SENSOR_VISION / 5);

    for (i = 0; i < 5; i++)
    {
        ptr = head_store;
        //xDir = round(robotCentreX+(-ROBOT_WIDTH/2)*cos((robot->angle)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*sin((robot->angle)*PI/180));
        //yDir = round(robotCentreY+(-ROBOT_WIDTH/2)*sin((robot->angle)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*cos((robot->angle)*PI/180));
        xDir = round(robotCentreX + (+ROBOT_WIDTH / 2) * cos((robot->angle - 90) * PI / 180) - (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensorSensitivityLength * i) * sin((robot->angle - 90) * PI / 180));
        yDir = round(robotCentreY + (+ROBOT_WIDTH / 2) * sin((robot->angle - 90) * PI / 180) + (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensorSensitivityLength * i) * cos((robot->angle - 90) * PI / 180));
        xTL = (int)xDir;
        yTL = (int)yDir;
        hit = 0;

        while (ptr != NULL) {
            hit = (hit || checkRobotSensor(xTL, yTL, sensorSensitivityLength, &ptr->wall));
            ptr = ptr->next;
        }
        if (hit)
            score = i;
    }
    return score;
}

int checkRobotSensorRightAllWalls(struct Robot* robot, struct Wall_collection* head) {
    struct Wall_collection* ptr, * head_store;
    int i;
    double xDir, yDir;
    int robotCentreX, robotCentreY, xTL, yTL;
    int score, hit;
    int sensorSensitivityLength;

    head_store = head;
    robotCentreX = robot->x + ROBOT_WIDTH / 2;
    robotCentreY = robot->y + ROBOT_HEIGHT / 2;
    score = 0;
    sensorSensitivityLength = floor(SENSOR_VISION / 5);

    for (i = 0; i < 5; i++)
    {
        ptr = head_store;
        //xDir = round(robotCentreX+(-ROBOT_WIDTH/2)*cos((robot->angle)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*sin((robot->angle)*PI/180));
        //yDir = round(robotCentreY+(-ROBOT_WIDTH/2)*sin((robot->angle)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*cos((robot->angle)*PI/180));
        xDir = round(robotCentreX + (-ROBOT_WIDTH / 2) * cos((robot->angle + 90) * PI / 180) - (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensorSensitivityLength * i) * sin((robot->angle + 90) * PI / 180));
        yDir = round(robotCentreY + (-ROBOT_WIDTH / 2) * sin((robot->angle + 90) * PI / 180) + (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensorSensitivityLength * i) * cos((robot->angle + 90) * PI / 180));
        xTL = (int)xDir;
        yTL = (int)yDir;
        hit = 0;

        while (ptr != NULL) {
            hit = (hit || checkRobotSensor(xTL, yTL, sensorSensitivityLength, &ptr->wall));
            ptr = ptr->next;
        }
        if (hit)
            score = i;
    }
    return score;
}

void robotUpdate(struct SDL_Renderer* renderer, struct Robot* robot) {
    double xDir, yDir;

    int robotCentreX, robotCentreY, xTR, yTR, xTL, yTL, xBR, yBR, xBL, yBL;
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);

    /*
    //Other Display options:
    // The actual square which the robot is tested against (not so nice visually with turns, but easier
    // to test overlap
    SDL_Rect rect = {robot->x, robot->y, robot->height, robot->width};
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    SDL_RenderDrawRect(renderer, &rect);
    SDL_RenderFillRect(renderer, &rect);
    */
    /*
    //Center Line of Robot. Line shows the direction robot is facing
    xDir = -30 * sin(-robot->angle*PI/180);
    yDir = -30 * cos(-robot->angle*PI/180);
    xDirInt = robot->x+ROBOT_WIDTH/2+ (int) xDir;
    yDirInt = robot->y+ROBOT_HEIGHT/2+ (int) yDir;
    SDL_RenderDrawLine(renderer,robot->x+ROBOT_WIDTH/2, robot->y+ROBOT_HEIGHT/2, xDirInt, yDirInt);
    */

    //Rotating Square
    //Vector rotation to work out corners x2 = x1cos(angle)-y1sin(angle), y2 = x1sin(angle)+y1cos(angle)
    robotCentreX = robot->x + ROBOT_WIDTH / 2;
    robotCentreY = robot->y + ROBOT_HEIGHT / 2;

    xDir = round(robotCentreX + (ROBOT_WIDTH / 2) * cos((robot->angle) * PI / 180) - (-ROBOT_HEIGHT / 2) * sin((robot->angle) * PI / 180));
    yDir = round(robotCentreY + (ROBOT_WIDTH / 2) * sin((robot->angle) * PI / 180) + (-ROBOT_HEIGHT / 2) * cos((robot->angle) * PI / 180));
    xTR = (int)xDir;
    yTR = (int)yDir;

    xDir = round(robotCentreX + (ROBOT_WIDTH / 2) * cos((robot->angle) * PI / 180) - (ROBOT_HEIGHT / 2) * sin((robot->angle) * PI / 180));
    yDir = round(robotCentreY + (ROBOT_WIDTH / 2) * sin((robot->angle) * PI / 180) + (ROBOT_HEIGHT / 2) * cos((robot->angle) * PI / 180));
    xBR = (int)xDir;
    yBR = (int)yDir;

    xDir = round(robotCentreX + (-ROBOT_WIDTH / 2) * cos((robot->angle) * PI / 180) - (ROBOT_HEIGHT / 2) * sin((robot->angle) * PI / 180));
    yDir = round(robotCentreY + (-ROBOT_WIDTH / 2) * sin((robot->angle) * PI / 180) + (ROBOT_HEIGHT / 2) * cos((robot->angle) * PI / 180));
    xBL = (int)xDir;
    yBL = (int)yDir;

    xDir = round(robotCentreX + (-ROBOT_WIDTH / 2) * cos((robot->angle) * PI / 180) - (-ROBOT_HEIGHT / 2) * sin((robot->angle) * PI / 180));
    yDir = round(robotCentreY + (-ROBOT_WIDTH / 2) * sin((robot->angle) * PI / 180) + (-ROBOT_HEIGHT / 2) * cos((robot->angle) * PI / 180));
    xTL = (int)xDir;
    yTL = (int)yDir;

    SDL_RenderDrawLine(renderer, xTR, yTR, xBR, yBR);
    SDL_RenderDrawLine(renderer, xBR, yBR, xBL, yBL);
    SDL_RenderDrawLine(renderer, xBL, yBL, xTL, yTL);
    SDL_RenderDrawLine(renderer, xTL, yTL, xTR, yTR);

    //Front Centre Sensor
    int sensor_sensitivity = floor(SENSOR_VISION / 5);
    int i;
    for (i = 0; i < 5; i++)
    {
        xDir = round(robotCentreX - (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensor_sensitivity * i) * sin((robot->angle) * PI / 180));
        yDir = round(robotCentreY + (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensor_sensitivity * i) * cos((robot->angle) * PI / 180));
        xTL = (int)xDir;
        yTL = (int)yDir;

        SDL_Rect rect = { xTL, yTL, 2, sensor_sensitivity };
        SDL_SetRenderDrawColor(renderer, 80 + (20 * (5 - i)), 80 + (20 * (5 - i)), 80 + (20 * (5 - i)), 255);
        SDL_RenderDrawRect(renderer, &rect);
        SDL_RenderFillRect(renderer, &rect);
    }

    //Left Sensor
    for (i = 0; i < 5; i++)
    {
        xDir = round(robotCentreX + (+ROBOT_WIDTH / 2) * cos((robot->angle - 90) * PI / 180) - (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensor_sensitivity * i) * sin((robot->angle - 90) * PI / 180));
        yDir = round(robotCentreY + (+ROBOT_WIDTH / 2) * sin((robot->angle - 90) * PI / 180) + (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensor_sensitivity * i) * cos((robot->angle - 90) * PI / 180));
        xTL = (int)xDir;
        yTL = (int)yDir;

        SDL_Rect rect = { xTL, yTL, 2, sensor_sensitivity };
        SDL_SetRenderDrawColor(renderer, 80 + (20 * (5 - i)), 80 + (20 * (5 - i)), 80 + (20 * (5 - i)), 255);
        SDL_RenderDrawRect(renderer, &rect);
        SDL_RenderFillRect(renderer, &rect);
    }

    //Right Sensor
    for (i = 0; i < 5; i++)
    {
        xDir = round(robotCentreX + (-ROBOT_WIDTH / 2) * cos((robot->angle + 90) * PI / 180) - (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensor_sensitivity * i) * sin((robot->angle + 90) * PI / 180));
        yDir = round(robotCentreY + (-ROBOT_WIDTH / 2) * sin((robot->angle + 90) * PI / 180) + (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensor_sensitivity * i) * cos((robot->angle + 90) * PI / 180));
        xTL = (int)xDir;
        yTL = (int)yDir;

        SDL_Rect rect = { xTL, yTL, 2, sensor_sensitivity };
        SDL_SetRenderDrawColor(renderer, 80 + (20 * (5 - i)), 80 + (20 * (5 - i)), 80 + (20 * (5 - i)), 255);
        SDL_RenderDrawRect(renderer, &rect);
        SDL_RenderFillRect(renderer, &rect);
    }

    //xDir = round(robotCentreX+(ROBOT_WIDTH/2-2)*cos((robot->angle)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*sin((robot->angle)*PI/180));
    //yDir = round(robotCentreY+(ROBOT_WIDTH/2-2)*sin((robot->angle)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*cos((robot->angle)*PI/180));
}

void robotMotorMove(struct Robot* robot, int crashed) {
    double x_offset, y_offset;
    if (crashed)
        robot->currentSpeed = 0;
    else {
        switch (robot->direction) {
        case UP:
            robot->currentSpeed += DEFAULT_SPEED_CHANGE;
            if (robot->currentSpeed > MAX_ROBOT_SPEED)
                robot->currentSpeed = MAX_ROBOT_SPEED;
            break;
        case DOWN:
            robot->currentSpeed -= DEFAULT_SPEED_CHANGE;
            if (robot->currentSpeed < -MAX_ROBOT_SPEED)
                robot->currentSpeed = -MAX_ROBOT_SPEED;
            break;
        case LEFT:
            robot->angle = (robot->angle + 360 - DEFAULT_ANGLE_CHANGE) % 360;
            break;
        case RIGHT:
            robot->angle = (robot->angle + DEFAULT_ANGLE_CHANGE) % 360;
            break;
        }
    }
    robot->direction = 0;
    x_offset = (-robot->currentSpeed * sin(-robot->angle * PI / 180));
    y_offset = (-robot->currentSpeed * cos(-robot->angle * PI / 180));

    robot->true_x += x_offset;
    robot->true_y += y_offset;

    x_offset = round(robot->true_x);
    y_offset = round(robot->true_y);

    robot->x = (int)x_offset;
    robot->y = (int)y_offset;
}

bool pidSimulation(struct Robot *robot, struct Wall_collection *head, int pid1, int pid2) {
    struct Robot robot1 = *robot;
    struct Robot robot2 = *robot;
    //ROBOT1 (pid candidate)
    if (pid1 > DEFAULT_ANGLE_CHANGE)
        pid1 = DEFAULT_ANGLE_CHANGE;
    if (pid1 < -DEFAULT_ANGLE_CHANGE)
        pid1 = -DEFAULT_ANGLE_CHANGE;
    robot1.total_dir_change += pid1;
    if (robot1.switch_hand == 0)
        robot1.angle = (robot1.angle + pid1) % 360;
    else
        robot1.angle = (robot1.angle - pid1) % 360;

    //ROBOT2 (best pid so far)
    if (pid2 > DEFAULT_ANGLE_CHANGE)
        pid2 = DEFAULT_ANGLE_CHANGE;
    if (pid2 < -DEFAULT_ANGLE_CHANGE)
        pid2 = -DEFAULT_ANGLE_CHANGE;
    robot2.total_dir_change += pid2;
    if (robot2.switch_hand == 0)
        robot2.angle = (robot2.angle + pid2) % 360;
    else
        robot2.angle = (robot2.angle - pid2) % 360;

    //Calculate new sensors for 2 
    int desired = 1, sensor1 = 0, sensor2 = 0;
    if (robot1.switch_hand == 1)
        sensor1 = checkRobotSensorRightAllWalls(&robot, head);
    else
        sensor1 = checkRobotSensorLeftAllWalls(&robot, head);
    if (robot2.switch_hand == 1)
        sensor2 = checkRobotSensorRightAllWalls(&robot, head);
    else
        sensor2 = checkRobotSensorLeftAllWalls(&robot, head);

    if (abs(sensor1 - desired) < abs(sensor2 - desired)) //new one better than the current pid 
        return true;

    if ((abs(sensor1 - desired) == abs(sensor2 - desired)) && (pid1 < pid2)) //new one better than the current pid 
        return true;

    return false;
}

void robotAutoMotorMove(struct Robot* robot, struct Wall_collection* head, int front_left_sensor, int front_right_sensor, int front_sensor) {

    int max_speed = 4;
    // printf("%d %d %d\n", *dir * 90 + 45, robot->angle, front_right_sensor);
     //clock_t curtime = clock();
     //float delta = curtime - robot->prev_time;
     //delta = delta / 1e3; // convert to second
     //robot->prev_time = curtime;

    if (front_left_sensor > 0 && robot->found_wall == 0) {
        robot->switch_hand = 1;
        robot->found_wall = 1;
    }
    if (front_sensor > 0 || front_right_sensor > 0) {
        robot->found_wall = 1;
    }

    if (robot->found_wall == 0) {
        if (robot->total_dir_change <= 15) {
            robot->angle = robot->angle + DEFAULT_ANGLE_CHANGE;
            robot->total_dir_change += DEFAULT_ANGLE_CHANGE;
        }
        if (robot->currentSpeed < max_speed) {
            robot->direction = UP;
        }
    }

    if (front_sensor > 0) {
        //robot->direction = DOWN;
        if (front_sensor > 0) {
            if (robot->switch_hand == 0)
                robot->angle = (robot->angle - DEFAULT_ANGLE_CHANGE) % 360;
            else
                robot->angle = (robot->angle + DEFAULT_ANGLE_CHANGE) % 360;
        }
    }
    else if (robot->found_wall == 1 && front_sensor < 1) {
        int sensor = front_right_sensor;
        if (robot->switch_hand == 1) {
            sensor = front_left_sensor;
            int other_sensor = front_right_sensor;
        }

        if (robot->currentSpeed < max_speed) {
            robot->direction = UP;
        }
        int error = robot->desired - sensor;
        double propotion = error * robot->kp;
        robot->kiTotal += error;
        float integral = robot->ki * (robot->kiTotal);
        float derivative = robot->kd * (error - robot->prior_error);
        if (derivative == 0 && error == 0)
            robot->kiTotal = 0;
        robot->prior_error = error;
        int pid_res = round(propotion + integral + derivative);
        //TWIDDLE
        double threshold = 1.5;
        int p[3] = {robot->kp, robot->ki, robot->kd};
        int dp[3] = {1, 1, 1};
        double bestErr = pid_res;
        struct Robot tmp_robot = *robot;
        struct Wall_collection ptr = *head;
        while (dp[0] + dp[1] + dp[2] > 2.9)
        {
            bool changed = false;
            for (int i = 0; i < 3; i++) {
                p[i] += dp[i];
                robot->kp = p[0];
                robot->ki = p[1];
                robot->kd = p[2];
                changed = true;
                double err = error * robot->kp + robot->ki * (robot->kiTotal) + robot->kd * (error - robot->prior_error);
                if (pidSimulation(&tmp_robot, &ptr, round(err), round(bestErr))) {
                    bestErr = fabs(err);
                    dp[i] *= 1.1;
                }
                else {
                    p[i] -= 2 * dp[i];
                    robot->kp = p[0];
                    robot->ki = p[1];
                    robot->kd = p[2];
                    changed = true;
                    err = error * robot->kp + robot->ki * (robot->kiTotal) + robot->kd * (error - robot->prior_error);
                    if (pidSimulation(&tmp_robot, &ptr, round(err), round(bestErr))) {
                        bestErr = abs(err);
                        dp[i] *= 1.1;
                    }
                    else {
                        p[i] += dp[i];
                        dp[i] *= 0.9;
                    }
                }
            }
            if (!changed) break;
        }
        //TWIDDLE*/
        pid_res = error * robot->kp + robot->ki * (robot->kiTotal) + robot->kd * (error - robot->prior_error);
        printf("%d\n", pid_res);
        if (pid_res > DEFAULT_ANGLE_CHANGE)
            pid_res = DEFAULT_ANGLE_CHANGE;
        if (pid_res < -DEFAULT_ANGLE_CHANGE)
            pid_res = -DEFAULT_ANGLE_CHANGE;
        printf("%d\n", pid_res);
        robot->total_dir_change += pid_res;
        if (robot->switch_hand == 0)
            robot->angle = (robot->angle + pid_res) % 360;
        else
            robot->angle = (robot->angle - pid_res) % 360;

        robot->total_err += abs(error);
        //printf("%d %f %f", robot -> currentSpeed, error, robot->prior_error);
        //printf("%d %f\n %d", robot->total_err, robot->kiTotal, robot->found_wall);
    }

    if (robot->found_wall == 1) {
        robot->kp = 8;
    }
}
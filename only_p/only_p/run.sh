#!/bin/bash

#!/usr/bin/bash

gcc wall.c formulas.c robot.c main.c -o main `sdl2-config --cflags --libs` -lm

./main

#!/bin/bash

cc -shared -o libplug.so -fPIC plug.c &&
cc -DHOT_RELOAD main.c hot_reload.c -o main -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -L. -lplug -Wl,-rpath=. -Wall

# cc  main.c -o main -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -L. -lplug -Wl,-rpath=. -Wall

# x86_64-w64-mingw32-gcc main.c plug.c -o main.exe -Iraylib-5.5_win64_mingw-w64/include/ -lraylib -lwinmm -lgdi32 -Lraylib-5.5_win64_mingw-w64/lib/ -Wall

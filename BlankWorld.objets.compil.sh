#!/bin/bash
g++ -Wall -c ./BlankWorld.objets.cpp -lrtaudio -std=c++11 &
g++ -Wall -o ./BlankWorld.objets ./BlankWorld.objets.cpp point.cpp Spectrum.GLShader.cpp -lrtaudio -lglfw -lGLEW -lGL -std=c++11 &


MSG=salut
echo $MSG
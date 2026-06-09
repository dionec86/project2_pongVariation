#ifndef CS3113_H
#define CS3113_H
#define LOG(argument) std::cout << argument << '\n'

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <string>
#include <iostream>

enum AppStatus   { TERMINATED, RUNNING };
enum GameStatus { NOT_STARTED, STARTED, FINISHED };
enum TextureType { SINGLE, ATLAS       };
enum DifficultyLevel { EASY, MEDIUM, HARD };
enum BotMode { OFF, AUTO, BOT };

Color ColorFromHex(const char *hex);
void Normalise(Vector2 *vector);
float GetLength(const Vector2 vector);
Rectangle getUVRectangle(const Texture2D *texture, int index, int rows, int cols);

#endif // CS3113_H
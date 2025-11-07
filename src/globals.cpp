#include "globals.h"

float externs::deltaT;

int externs::screenWidth = 1250;
int externs::screenHeight = 768;

bool externs::retry = false;
bool externs::hasLost = false;

std::string externs::backgroundFrontTexture = "res/placeholder_front.png";
int externs::backgroundFrontTextureID = 0;

std::string externs::backgroundMiddleTexture = "res/placeholder_middle.png";
int externs::backgroundMiddleTextureID = 0;

std::string externs::backgroundBackTexture = "res/placeholder_back.png";
int externs::backgroundBackTextureID = 0;

#pragma once
#include <string>

namespace externs
{
	extern float deltaT;

	extern int screenWidth;
	extern int screenHeight;

	extern bool retry;
	extern bool hasLost;

	extern std::string backgroundFrontTexture;
	extern int backgroundFrontTextureID;

	extern std::string backgroundMiddleTexture;
	extern int backgroundMiddleTextureID;

	extern std::string backgroundBackTexture;
	extern int backgroundBackTextureID;
}

namespace globals
{
	const float acceleration = 981.0f;
}
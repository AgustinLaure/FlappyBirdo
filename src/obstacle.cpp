#include "obstacle.h"

#include "globals.h"

namespace variables
{
	static float velocity = 500.0f;
	static float offSet = 250.0f;
}

void obstacle::Initialization(Obstacle& obstacle)
{
	obstacle.textureID = 0;

	obstacle.width = 50.0f;
	obstacle.height = 400.0f;

	obstacle.bottom = { static_cast<float>(externs::screenWidth) - obstacle.width,  static_cast<float>(externs::screenHeight) - obstacle.height };
	obstacle.top = { obstacle.bottom.x,  (-obstacle.bottom.y + obstacle.height) - variables::offSet};

	obstacle.velocity = variables::velocity;
}

void obstacle::Update(Obstacle& obstacle)
{
	obstacle.bottom.x += -obstacle.velocity * externs::deltaT;
	obstacle.top.x = obstacle.bottom.x;

	if (obstacle::CheckOutOfBounds(obstacle))
	{
		obstacle.bottom = { static_cast<float>(externs::screenWidth) - obstacle.width, static_cast<float>(GetRandomValue(0, externs::screenHeight / 2)) - obstacle.height };
		obstacle.top = { obstacle.bottom.x, static_cast<float>(-(GetRandomValue(externs::screenHeight / 2, externs::screenHeight)) + obstacle.height) - variables::offSet };
	}
}

void obstacle::Draw(Obstacle obstacle)
{
	DrawRectangle(static_cast<int>(obstacle.bottom.x), static_cast<int>(obstacle.bottom.y), static_cast<int>(obstacle.width), static_cast<int>(obstacle.height), RED);
	DrawRectangle(static_cast<int>(obstacle.top.x), static_cast<int>(obstacle.top.y), static_cast<int>(obstacle.width), static_cast<int>(obstacle.height), RED);
}

bool obstacle::CheckOutOfBounds(Obstacle& obstacle)
{
	return (obstacle.bottom.x + obstacle.width < 0.0f);		
}

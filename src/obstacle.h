#pragma once
#include "raylib.h"

namespace obstacle
{
	struct Obstacle
	{
		int textureID;

		Vector2 top;
		Vector2 bottom;
		
		float velocity;

		float width;
		float height;
	};

	void Initialization(Obstacle& obstacle);
	void Update(Obstacle& obstacle);
	void Draw(Obstacle obstacle);

	bool CheckOutOfBounds(Obstacle& obstacle);
}

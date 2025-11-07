#include "player.h"
#include "globals.h"

#include <iostream>

namespace variables
{
	static float velocity = 500.0f;
}

void player::Initialization(Bird& bird)
{
	bird.textureID = 0;

	bird.position = { static_cast<float>(externs::screenWidth) / 6.0f, static_cast<float>(externs::screenHeight) / 2.0f };

	bird.velocity = 0.0f;
}

void player::Update(Bird& bird)
{
	bird.velocity -= globals::acceleration * externs::deltaT;

	if (player::IsPlayerMoving())
	{
		bird.velocity = variables::velocity;
	}

	bird.position.y -= bird.velocity * externs::deltaT;

	CheckState(bird);
}

void player::Draw(Bird bird)
{
	DrawCircleV(bird.position, 30.0f, BLUE);
}

bool player::IsPlayerMoving()
{
	return IsKeyPressed(KEY_UP);
}

bool player::HasLost(Bird bird)
{
	return bird.position.y - (30.0f / 2.0f) > (externs::screenHeight + 30.0f);
}

bool player::IsTouchingCeiling(Bird bird)
{
	return (bird.position.y - 30.0f) < 0;
}

void player::CheckState(Bird& bird)
{
	if (HasLost(bird))
	{
		externs::hasLost = true;
	}
	if (IsTouchingCeiling(bird))
	{
		bird.position.y = 30.0f;
		bird.velocity = 0.0f;
	}
}

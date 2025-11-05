#include "gameloop.h"
#include "globals.h"
#include "player.h"
#include "obstacle.h"
#include "text.h"
#include "draw.h"
#include "utils.h"
#include "button.h"

#include <iostream>

#include "raylib.h"

namespace essentials
{
	static void GetDeltaTime();
}

namespace objects
{
	struct Cursor
	{
		float radius = 0.5f;
		float positionX = 0.0f;
		float positionY = 0.0f;
	};

	static Cursor cursor;

	static buttons::Button play;
	static buttons::Button credits;
	static buttons::Button exit;

	static void UpdateMousePosition(Cursor& cursor);

	static player::Bird bird;
	static obstacle::Obstacle obstacle;
}

namespace game
{
	namespace state
	{
		enum class State
		{
			Menu, Play, Credits, Pause, HowToPlay, EndScreen, Settings, Exit
		};
	}
	state::State gameState = state::State::Menu;

	void Initialize(player::Bird& bird, obstacle::Obstacle& obstacle, buttons::Button& credits, buttons::Button& play, buttons::Button& exit);
	void Update();
	void Draw();

	static bool CheckCollisions(player::Bird bird, obstacle::Obstacle obstacle);
	static void DrawCurrentVer();

	namespace menu
	{
		static void Update(buttons::Button& play, buttons::Button& credits, buttons::Button& exit, objects::Cursor& cursor, state::State& currentState);
		static void Draw(buttons::Button play, buttons::Button credits, buttons::Button exit);
	}

	namespace credits
	{
		static void Update(state::State& currentState, objects::Cursor& cursor, buttons::Button& returnButton);
		static void Draw(buttons::Button& returnButton);
	}
}

namespace text
{
	static text::Text version;
}

void game::GameLoop()
{
	game::Initialize(objects::bird, objects::obstacle, objects::play, objects::credits, objects::exit);

	while (!WindowShouldClose() && game::gameState != game::state::State::Exit)
	{
		switch (game::gameState)
		{
		case game::state::State::Menu:

			game::menu::Update(objects::play, objects::credits, objects::exit, objects::cursor, game::gameState);

			break;

		case game::state::State::Play:

			game::Update();

			break;

		case game::state::State::Credits:

			game::credits::Update(game::gameState, objects::cursor, objects::exit);

			break;
		default:
			break;
		}

		BeginDrawing();
		ClearBackground(RAYWHITE);

		switch (game::gameState)
		{
		case game::state::State::Menu:

			game::menu::Draw(objects::play, objects::credits, objects::exit);

			break;

		case game::state::State::Play:

			game::Draw();

			break;

		case game::state::State::Credits:

			game::credits::Draw(objects::exit);


			break;
		default:
			break;
		}
		DrawCircleV({ objects::cursor.positionX, objects::cursor.positionY },15,RED);

		Rectangle playButt = { objects::play.position.x - objects::play.width / 2, objects::play.position.y - objects::play.height / 2, objects::play.width, objects::play.height };
		Rectangle creditsButt = { objects::credits.position.x - objects::credits.width / 2, objects::credits.position.y - objects::credits.height / 2, objects::credits.width, objects::credits.height };

		DrawRectangleRec(playButt,MAGENTA);
		DrawRectangleRec(creditsButt,GOLD);

		EndDrawing();
	}

	CloseWindow();
}

void essentials::GetDeltaTime()
{
	externs::deltaT = GetFrameTime();
}

void game::Initialize(player::Bird& bird, obstacle::Obstacle& obstacle, buttons::Button& play, buttons::Button& credits, buttons::Button& exit)
{
	InitWindow(externs::screenWidth, externs::screenHeight, "Flappy Bird");

	float buttonWidth = 25.0f;
	float buttonHeight = 8.0f;
	float buttonCenterX = 50.0f;

	//play.text.font = externs::defaultText.font;
	play.text.text = "PLAY";
	buttons::Initialize(play, buttonWidth, buttonHeight, buttonCenterX, 35.0f);

	//credits.text.font = externs::defaultText.font;
	credits.text.text = "CREDITS";
	buttons::Initialize(credits, buttonWidth, buttonHeight, buttonCenterX, 50.0f);

	//exit.text.font = externs::defaultText.font;
	exit.text.text = "EXIT";
	buttons::Initialize(exit, buttonWidth, buttonHeight, buttonCenterX, 75.0f);

	player::Initialization(bird);
	obstacle::Initialization(obstacle);

}

void game::Update()
{
	essentials::GetDeltaTime();

	player::Update(objects::bird);
	obstacle::Update(objects::obstacle);

	if (game::CheckCollisions(objects::bird, objects::obstacle))
	{
		CloseWindow();
	}
}

void game::Draw()
{
	game::DrawCurrentVer();
	player::Draw(objects::bird);
	obstacle::Draw(objects::obstacle);
}

void game::menu::Update(buttons::Button& play, buttons::Button& credits, buttons::Button& exit, objects::Cursor& cursor, state::State& currentState)
{
	objects::UpdateMousePosition(cursor);

	Rectangle playButt = { play.position.x - play.width / 2, play.position.y - play.height / 2, play.width, play.height };

	if (CheckCollisionPointRec({ cursor.positionX, cursor.positionY }, playButt))
	{
		play.text.color = WHITE;

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			currentState = state::State::Play;
		}
	}
	else
	{
		play.text.color = GRAY;
	}

	Rectangle creditsButt = { credits.position.x - credits.width / 2, credits.position.y - credits.height / 2, credits.width, credits.height };

	if (CheckCollisionPointRec({ cursor.positionX, cursor.positionY }, creditsButt))
	{
		credits.text.color = WHITE;
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			currentState = state::State::Credits;
		}
	}
	else
	{
		credits.text.color = GRAY;
	}

	Rectangle exitButt = { exit.position.x - exit.width / 2, exit.position.y - exit.height / 2, exit.width, exit.height };


	if (CheckCollisionPointRec({ cursor.positionX, cursor.positionY }, exitButt))
	{
		exit.text.color = WHITE;
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			currentState = state::State::Exit;
		}
	}
	else
	{
		exit.text.color = GRAY;
	}
}

void game::menu::Draw(buttons::Button play, buttons::Button credits, buttons::Button exit)
{
	buttons::Draw(play);
	buttons::Draw(credits);
	buttons::Draw(exit);
}

void game::credits::Update(state::State& currentState, objects::Cursor& cursor, buttons::Button& returnButton)
{
	objects::UpdateMousePosition(cursor);
	returnButton.height = 0;

	currentState = game::state::State::EndScreen;

}

void game::credits::Draw(buttons::Button& returnButton)
{
	buttons::Draw(returnButton);
}

bool game::CheckCollisions(player::Bird bird, obstacle::Obstacle obstacle)
{
	const float circleRadius = 30.0f;
	float circleX = bird.position.x;
	float circleY = bird.position.y;

	float rectX = obstacle.position.x;
	float rectY = obstacle.position.y;
	float rectWidth = obstacle.width;
	float rectHeight = obstacle.height;

	float closestX = std::max(rectX, std::min(circleX, rectX + rectWidth));
	float closestY = std::max(rectY, std::min(circleY, rectY + rectHeight));

	float distanceX = circleX - closestX;
	float distanceY = circleY - closestY;

	float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

	if (distanceSquared < (circleRadius * circleRadius))
	{
		return true;
	}
	return false;
}

void game::DrawCurrentVer()
{
	text::version.text = "ver 0.1";
	text::version.posX = 95.0f;
	text::version.posY = 98.0f;
	text::version.color = BLACK;

	draw::DrawText(text::version);
}

void objects::UpdateMousePosition(Cursor& cursorParam)
{
	cursorParam.positionX = utils::PixelsToPercentX(GetMousePosition().x);
	cursorParam.positionY = utils::PixelsToPercentY(GetMousePosition().y);
};
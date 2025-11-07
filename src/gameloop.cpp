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

	static void Initialize(buttons::Button& credits, buttons::Button& play, buttons::Button& exit, Texture& tempTexture, int& backgroundFrontTextureID, int& backgroundMiddleTextureID, int& backgroundBackTextureID);
	static void Update();
	static void Draw();

	static bool CheckCollisionsCircleRectangle(float circleX, float circleY, float recX, float recY, float width, float height);
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

namespace assets
{
	static text::Text version;
	static Texture tempTexture;

	namespace parallax
	{
		static float scrollingBack = 0.0f;
		static float scrollingMid = 0.0f;
		static float scrollingFront = 0.0f;

		static void Update();
		static void Draw();
	}
}

void game::GameLoop()
{
	InitWindow(externs::screenWidth, externs::screenHeight, "Flappy Bird");

	game::Initialize(objects::play, objects::credits, objects::exit, assets::tempTexture,
		externs::backgroundFrontTextureID, externs::backgroundMiddleTextureID, externs::backgroundBackTextureID);
	player::Initialization(objects::bird);
	obstacle::Initialization(objects::obstacle);

	while (!WindowShouldClose() && game::gameState != game::state::State::Exit)
	{
		switch (game::gameState)
		{
		case game::state::State::Menu:

			game::menu::Update(objects::play, objects::credits, objects::exit, objects::cursor, game::gameState);

			if (externs::retry)
			{
				player::Initialization(objects::bird);
				obstacle::Initialization(objects::obstacle);

				externs::retry = false;
			}
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

		EndDrawing();
	}

	CloseWindow();
}

void essentials::GetDeltaTime()
{
	externs::deltaT = GetFrameTime();
}

void game::Initialize(buttons::Button& play, buttons::Button& credits, buttons::Button& exit, Texture& tempTexture, int& backgroundFrontTextureID, int& backgroundMiddleTextureID, int& backgroundBackTextureID)
{
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

	tempTexture = LoadTexture(externs::backgroundFrontTexture.c_str());
	backgroundFrontTextureID = tempTexture.id;

	tempTexture = LoadTexture(externs::backgroundMiddleTexture.c_str());
	backgroundMiddleTextureID = tempTexture.id;

	tempTexture = LoadTexture(externs::backgroundBackTexture.c_str());
	backgroundBackTextureID = tempTexture.id;

}

void game::Update()
{
	assets::parallax::Update();

	essentials::GetDeltaTime();

	player::Update(objects::bird);
	obstacle::Update(objects::obstacle);

	if (game::CheckCollisionsCircleRectangle(objects::bird.position.x, objects::bird.position.y, objects::obstacle.bottom.x,
		objects::obstacle.bottom.y, objects::obstacle.width, objects::obstacle.height) || game::CheckCollisionsCircleRectangle
		(objects::bird.position.x, objects::bird.position.y, objects::obstacle.top.x, objects::obstacle.top.y, objects::obstacle.width, objects::obstacle.height))
	{
		game::gameState = game::state::State::Menu;
		externs::retry = true;
	}

	if (externs::hasLost)
	{
		externs::retry = true;
		externs::hasLost = false;
		game::gameState = game::state::State::Menu;
	}
}

void game::Draw()
{
	assets::parallax::Draw();

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

	game::DrawCurrentVer();
}

void game::credits::Update(state::State& currentState, objects::Cursor& cursor, buttons::Button& returnButton)
{
	objects::UpdateMousePosition(cursor);

	Rectangle exitButt = { returnButton.position.x - returnButton.width / 2, returnButton.position.y - returnButton.height / 2, returnButton.width, returnButton.height };

	if (CheckCollisionPointRec({ cursor.positionX, cursor.positionY }, exitButt))
	{
		returnButton.text.color = WHITE;
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			currentState = state::State::Menu;
		}
	}
	else
	{
		returnButton.text.color = GRAY;
	}
}

void game::credits::Draw(buttons::Button& returnButton)
{
	text::Text credits;
	credits.text = "Made by Eluney Jazmin Mousseigne";
	credits.posX = 50;
	credits.posY = 35;
	credits.fonstSize = 50;
	credits.color = BLACK;

	draw::DrawText(credits);
	buttons::Draw(returnButton);
	game::DrawCurrentVer();
}

bool game::CheckCollisionsCircleRectangle(float circleX, float circleY, float recX, float recY, float width, float height)
{
	const float circleRadius = 30.0f;

	float closestX = std::max(recX, std::min(circleX, recX + width));
	float closestY = std::max(recY, std::min(circleY, recY + height));

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
	assets::version.text = "ver 0.3";
	assets::version.posX = 95.0f;
	assets::version.posY = 98.0f;
	assets::version.color = BLACK;

	draw::DrawText(assets::version);
}

void objects::UpdateMousePosition(Cursor& cursorParam)
{
	cursorParam.positionX = utils::PixelsToPercentX(GetMousePosition().x);
	cursorParam.positionY = utils::PixelsToPercentY(GetMousePosition().y);
};

void assets::parallax::Update()
{
	scrollingBack -= 1.0f * externs::deltaT;
	scrollingMid -= 2.0f * externs::deltaT;
	scrollingFront -= 4.0f * externs::deltaT;

	if (scrollingBack <= -200.0f)
	{
		scrollingBack = 0;
	}
	if (scrollingMid <= -200.0f)
	{
		scrollingMid = 0;
	}
	if (scrollingFront <= -200.0f)
	{
		scrollingFront = 0;
	}
}

void assets::parallax::Draw()
{
	draw::DrawSpriteEx(static_cast<float>(externs::backgroundBackTextureID), scrollingBack, 0.0f, 200.0f, 100.0f, 0.0f, WHITE);
	draw::DrawSpriteEx(static_cast<float>(externs::backgroundBackTextureID), 200.0f + scrollingBack, 0.0f, 200.0f, 100.0f, 0.0f, WHITE);

	draw::DrawSpriteEx(static_cast<float>(externs::backgroundMiddleTextureID), scrollingMid, 0.0f, 200.0f, 100.0f, 0.0f, WHITE);
	draw::DrawSpriteEx(static_cast<float>(externs::backgroundMiddleTextureID), 200.0f + scrollingMid, 0.0f, 200.0f, 100.0f, 0.0f, WHITE);

	draw::DrawSpriteEx(static_cast<float>(externs::backgroundFrontTextureID), scrollingFront, 0.0f, 200.0f, 100.0f, 0.0f, WHITE);
	draw::DrawSpriteEx(static_cast<float>(externs::backgroundFrontTextureID), 200.0f + scrollingFront, 0.0f, 200.0f, 100.0f, 0.0f, WHITE);
}
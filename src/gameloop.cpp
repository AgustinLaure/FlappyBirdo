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

	static buttons::Button singleplayer;
	static buttons::Button multiplayer;
	static buttons::Button credits;
	static buttons::Button exit;

	static void UpdateMousePosition(Cursor& cursor);

	static player::Bird bird1;
	static player::Bird bird2;
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

		enum class PlayStyle
		{
			Singleplayer, Multiplayer
		};
	}
	state::State gameState = state::State::Menu;
	state::PlayStyle playStyle = state::PlayStyle::Singleplayer;

	static void Initialize(buttons::Button& singleplayer, buttons::Button& multiplayer, buttons::Button& credits, buttons::Button& exit, Texture& tempTexture, int& backgroundFrontTextureID, int& backgroundMiddleTextureID, int& backgroundBackTextureID);
	static void Update(state::PlayStyle currentPlayStyle);
	static void Draw(state::PlayStyle currentPlayStyle);

	static bool CheckCollisionsCircleRectangle(float circleX, float circleY, float recX, float recY, float width, float height);
	static void DrawCurrentVer();

	namespace menu
	{
		static void Update(buttons::Button& singleplayer, buttons::Button& multiplayer, buttons::Button& credits, buttons::Button& exit, objects::Cursor& cursor, state::State& currentState, state::PlayStyle& currentPlayStyle);
		static void Draw(buttons::Button singeplayer, buttons::Button multiplayer, buttons::Button credits, buttons::Button exit);
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

	game::Initialize(objects::singleplayer, objects::multiplayer, objects::credits, objects::exit, assets::tempTexture,
		externs::backgroundFrontTextureID, externs::backgroundMiddleTextureID, externs::backgroundBackTextureID);
	player::Initialization(objects::bird1, KEY_W, { static_cast<float>(externs::screenWidth) / 6.0f, static_cast<float>(externs::screenHeight) / 2.0f });
	player::Initialization(objects::bird2, KEY_UP, { static_cast<float>(externs::screenWidth) / 5.0f, static_cast<float>(externs::screenHeight) / 2.0f });
	obstacle::Initialization(objects::obstacle);

	while (!WindowShouldClose() && game::gameState != game::state::State::Exit)
	{
		switch (game::gameState)
		{
		case game::state::State::Menu:

			game::menu::Update(objects::singleplayer, objects::multiplayer, objects::credits, objects::exit, objects::cursor, game::gameState, game::playStyle);

			if (externs::retry)
			{
				player::Initialization(objects::bird1, KEY_W, { static_cast<float>(externs::screenWidth) / 6.0f, static_cast<float>(externs::screenHeight) / 2.0f });
				player::Initialization(objects::bird2, KEY_UP, { static_cast<float>(externs::screenWidth) / 5.0f, static_cast<float>(externs::screenHeight) / 2.0f });
				obstacle::Initialization(objects::obstacle);

				externs::retry = false;
			}
			break;

		case game::state::State::Play:

			game::Update(game::playStyle);

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

			game::menu::Draw(objects::singleplayer, objects::multiplayer, objects::credits, objects::exit);

			break;

		case game::state::State::Play:

			game::Draw(game::playStyle);

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

void game::Initialize(buttons::Button& singleplayer, buttons::Button& multiplayer, buttons::Button& credits, buttons::Button& exit, Texture& tempTexture, int& backgroundFrontTextureID, int& backgroundMiddleTextureID, int& backgroundBackTextureID)
{
	float buttonWidth = 25.0f;
	float buttonHeight = 8.0f;
	float buttonCenterX = 50.0f;

	//play.text.font = externs::defaultText.font;
	singleplayer.text.text = "SINGLEPLAYER";
	buttons::Initialize(singleplayer, buttonWidth, buttonHeight, buttonCenterX, 25.0f);

	multiplayer.text.text = "MULTIPLAYER";
	buttons::Initialize(multiplayer, buttonWidth, buttonHeight, buttonCenterX, 35.0f);

	//credits.text.font = externs::defaultText.font;
	credits.text.text = "CREDITS";
	buttons::Initialize(credits, buttonWidth, buttonHeight, buttonCenterX, 45.0f);

	//exit.text.font = externs::defaultText.font;
	exit.text.text = "EXIT";
	buttons::Initialize(exit, buttonWidth, buttonHeight, buttonCenterX, 55.0f);

	tempTexture = LoadTexture(externs::backgroundFrontTexture.c_str());
	backgroundFrontTextureID = tempTexture.id;

	tempTexture = LoadTexture(externs::backgroundMiddleTexture.c_str());
	backgroundMiddleTextureID = tempTexture.id;

	tempTexture = LoadTexture(externs::backgroundBackTexture.c_str());
	backgroundBackTextureID = tempTexture.id;

}

void game::Update(state::PlayStyle currentPlayStyle)
{
	if (!externs::hasReadRules)
	{
		if (IsKeyPressed(KEY_ENTER))
		{
			externs::hasReadRules = true;
		}
	}
	else
	{
		assets::parallax::Update();

		essentials::GetDeltaTime();

		if (objects::bird1.isOn)
		{
			player::Update(objects::bird1);
		}

		if (currentPlayStyle == state::PlayStyle::Multiplayer)
		{
			if (objects::bird2.isOn)
			{
				player::Update(objects::bird2);
			}
		}

		obstacle::Update(objects::obstacle);

		if (objects::bird1.isOn)
		{
			if (game::CheckCollisionsCircleRectangle(objects::bird1.position.x, objects::bird1.position.y, objects::obstacle.bottom.x,
				objects::obstacle.bottom.y, objects::obstacle.width, objects::obstacle.height) || game::CheckCollisionsCircleRectangle
				(objects::bird1.position.x, objects::bird1.position.y, objects::obstacle.top.x, objects::obstacle.top.y, objects::obstacle.width, objects::obstacle.height))
			{
				objects::bird1.isOn = false;
				externs::retry = true;
			}
		}

		if (objects::bird2.isOn)
		{
			if (game::CheckCollisionsCircleRectangle(objects::bird2.position.x, objects::bird2.position.y, objects::obstacle.bottom.x,
				objects::obstacle.bottom.y, objects::obstacle.width, objects::obstacle.height) || game::CheckCollisionsCircleRectangle
				(objects::bird2.position.x, objects::bird2.position.y, objects::obstacle.top.x, objects::obstacle.top.y, objects::obstacle.width, objects::obstacle.height))
			{
				objects::bird2.isOn = false;
				externs::retry = true;
			}
		}

		if (!objects::bird1.isOn)
		{
			if (currentPlayStyle == state::PlayStyle::Multiplayer)
			{
				if (!objects::bird2.isOn)
				{
					externs::hasLost = true;
				}
			}
			else
			{
				externs::hasLost = true;
			}
		}

		if (externs::hasLost)
		{
			externs::retry = true;
			externs::hasLost = false;
			game::gameState = game::state::State::Menu;
		}
	}
}

void game::Draw(state::PlayStyle currentPlayStyle)
{
	if (!externs::hasReadRules)
	{
		if (currentPlayStyle == state::PlayStyle::Multiplayer)
		{
			std::string player1Text = "Player1";
			int player1TextFont = 30;
			DrawText(player1Text.c_str(), externs::screenWidth / 2 - 200 - MeasureText(player1Text.c_str(), player1TextFont) / 2, externs::screenHeight / 5, player1TextFont, BLACK);

			std::string player2Text = "Player2";
			int player2TextFont = 30;
			DrawText(player2Text.c_str(), externs::screenWidth / 2 + 200 - MeasureText(player2Text.c_str(), player2TextFont) / 2, externs::screenHeight / 5, player2TextFont, BLACK);

			std::string player1ControlText = "-W to jump";
			int player1ControlTextFont = 22;
			DrawText(player1ControlText.c_str(), externs::screenWidth / 2 - 200 - MeasureText(player1ControlText.c_str(), player1ControlTextFont) / 2, externs::screenHeight / 2 - 50, player1ControlTextFont, BLACK);

			std::string player2ControlText = "-Up arrow to jump";
			int player2ControlTextFont = 22;
			DrawText(player2ControlText.c_str(), externs::screenWidth / 2 + 200 - MeasureText(player2ControlText.c_str(), player2ControlTextFont) / 2, externs::screenHeight / 2 - 50, player2ControlTextFont, BLACK);
		}
		else
		{
			std::string player1Text = "Player";
			int player1TextFont = 30;
			DrawText(player1Text.c_str(), externs::screenWidth / 2 - MeasureText(player1Text.c_str(), player1TextFont) / 2, externs::screenHeight / 5, player1TextFont, BLACK);

			std::string player1ControlText = "-W to jump";
			int player1ControlTextFont = 22;
			DrawText(player1ControlText.c_str(), externs::screenWidth / 2 - MeasureText(player1ControlText.c_str(), player1ControlTextFont) / 2, externs::screenHeight / 2 - 50, player1ControlTextFont, BLACK);
		}

		std::string continueText = "Press ENTER to continue";
		int continueTextFont = 30;
		DrawText(continueText.c_str(), externs::screenWidth / 2 - MeasureText(continueText.c_str(), continueTextFont) / 2, externs::screenHeight / 2 + 150, continueTextFont, BLACK);
	}
	else
	{
		assets::parallax::Draw();

		if (objects::bird1.isOn)
		{
			player::Draw(objects::bird1);
		}

		if (currentPlayStyle == game::state::PlayStyle::Multiplayer)
		{
			if (objects::bird2.isOn)
			{
				player::Draw(objects::bird2);
			}
		}

		obstacle::Draw(objects::obstacle);
	}
}

void game::menu::Update(buttons::Button& singleplayer, buttons::Button& multiplayer, buttons::Button& credits, buttons::Button& exit, objects::Cursor& cursor, state::State& currentState, state::PlayStyle& currentPlayStyle)
{
	objects::UpdateMousePosition(cursor);

	Rectangle singleplayerButt = { singleplayer.position.x - singleplayer.width / 2, singleplayer.position.y - singleplayer.height / 2, singleplayer.width, singleplayer.height };

	if (CheckCollisionPointRec({ cursor.positionX, cursor.positionY }, singleplayerButt))
	{
		singleplayer.text.color = WHITE;

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			currentState = state::State::Play;
			currentPlayStyle = state::PlayStyle::Singleplayer;
		}
	}
	else
	{
		singleplayer.text.color = GRAY;
	}

	Rectangle multiplayerButt = { multiplayer.position.x - multiplayer.width / 2, multiplayer.position.y - multiplayer.height / 2, multiplayer.width, multiplayer.height };

	if (CheckCollisionPointRec({ cursor.positionX, cursor.positionY }, multiplayerButt))
	{
		multiplayer.text.color = WHITE;

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			currentState = state::State::Play;
			currentPlayStyle = state::PlayStyle::Multiplayer;
		}
	}
	else
	{
		multiplayer.text.color = GRAY;
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

void game::menu::Draw(buttons::Button singeplayer, buttons::Button multiplayer, buttons::Button credits, buttons::Button exit)
{
	buttons::Draw(singeplayer);
	buttons::Draw(multiplayer);
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
	text::Text credits1;
	credits1.text = "Made by Eluney Jazmin Mousseigne";
	credits1.posX = 50;
	credits1.posY = 20;
	credits1.fonstSize = 40;
	credits1.color = BLACK;

	text::Text credits2;
	credits2.text = "&";
	credits2.posX = 50;
	credits2.posY = 30;
	credits2.fonstSize = 40;
	credits2.color = BLACK;

	text::Text credits3;
	credits3.text = "Agustin Ezequiel Laure";
	credits3.posX = 50;
	credits3.posY = 40;
	credits3.fonstSize = 40;
	credits3.color = BLACK;

	draw::DrawText(credits1);
	draw::DrawText(credits2);
	draw::DrawText(credits3);
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
	assets::version.text = "ver 0.4";
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
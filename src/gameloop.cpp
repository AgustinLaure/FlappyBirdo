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

namespace Game
{
	namespace Essentials
	{
		struct Cursor
		{
			float radius = 0.5f;
			float positionX = 0.0f;
			float positionY = 0.0f;
		};

		namespace Objects
		{
			static Cursor cursor;
		}

		static void UpdateMousePosition();
		static void GetDeltaTime();
	}

	namespace Assets
	{
		namespace Parallax
		{
			static float scrollingBack = 0.0f;
			static float scrollingMid = 0.0f;
			static float scrollingFront = 0.0f;

			static Text::Text version;
			static Texture tempTexture;

			static void Update();
			static void Draw();
		}
	}

	namespace Playing
	{
		namespace Objects
		{
			static Player::Bird bird1;
			static Player::Bird bird2;
			static Obstacle::Obstacle obstacle;

			static Buttons::Button retryButton;
			static Buttons::Button exitButton;
		}

		enum class Playstyle
		{
			Singleplayer, Multiplayer
		};

		enum class GameplayScene
		{
			ReadingRules,
			Playing,
			Pause,
			Finished
		};

		static Playstyle currentPlaystyle = Playstyle::Singleplayer;

		static GameplayScene currentScene = GameplayScene::ReadingRules;

		static void Update();
		static void Draw();

		static bool CheckCollisionsCircleRectangle(float circleX, float circleY, float recX, float recY, float width, float height);
	}

	namespace Menu
	{
		namespace Objects
		{
			static Buttons::Button singleplayer;
			static Buttons::Button multiplayer;
			static Buttons::Button credits;
			static Buttons::Button exit;
		}

		namespace Credits
		{
			namespace Objects
			{
				static Buttons::Button returnButton;
			}

			static void Update();
			static void Draw();
		}

		static void Update();
		static void Draw();
		static void DrawCurrentVer();
	}

	enum class State
	{
		Menu, Playing, Credits, HowToPlay, Settings, Exit
	};

	static State currentState = State::Menu;

	static void Initialize();

	namespace Playing
	{
		void Update()
		{
			switch (currentScene)
			{
			case GameplayScene::ReadingRules:
				if (IsKeyPressed(KEY_ENTER))
				{
					currentScene = GameplayScene::Playing;
				}
				break;

			case GameplayScene::Playing:
				Assets::Parallax::Update();

				Essentials::GetDeltaTime();

				if (Objects::bird1.isOn)
				{
					Player::Update(Objects::bird1);
				}

				if (currentPlaystyle == Playstyle::Multiplayer)
				{
					if (Objects::bird2.isOn)
					{
						Player::Update(Objects::bird2);
					}
				}

				Obstacle::Update(Objects::obstacle);

				if (Objects::bird1.isOn)
				{
					if (CheckCollisionsCircleRectangle(Objects::bird1.position.x, Objects::bird1.position.y, Objects::obstacle.bottom.x,
						Objects::obstacle.bottom.y, Objects::obstacle.width, Objects::obstacle.height) || CheckCollisionsCircleRectangle
						(Objects::bird1.position.x, Objects::bird1.position.y, Objects::obstacle.top.x, Objects::obstacle.top.y, Objects::obstacle.width, Objects::obstacle.height))
					{
						Objects::bird1.isOn = false;
					}
				}

				if (Objects::bird2.isOn)
				{
					if (CheckCollisionsCircleRectangle(Objects::bird2.position.x, Objects::bird2.position.y, Objects::obstacle.bottom.x,
						Objects::obstacle.bottom.y, Objects::obstacle.width, Objects::obstacle.height) || CheckCollisionsCircleRectangle
						(Objects::bird2.position.x, Objects::bird2.position.y, Objects::obstacle.top.x, Objects::obstacle.top.y, Objects::obstacle.width, Objects::obstacle.height))
					{
						Objects::bird2.isOn = false;
					}
				}

				if (!Objects::bird1.isOn)
				{
					if (currentPlaystyle == Playstyle::Multiplayer)
					{
						if (!Objects::bird2.isOn)
						{
							Externs::hasLost = true;
						}
					}
					else
					{
						Externs::hasLost = true;
					}
				}

				if (Externs::hasLost)
				{
					Externs::hasLost = false;
					currentScene = GameplayScene::Finished;
				}
				break;

			case GameplayScene::Pause:

			case GameplayScene::Finished:

				Essentials::UpdateMousePosition();

				if (CheckCollisionPointRec({ Essentials::Objects::cursor.positionX, Essentials::Objects::cursor.positionY }, { Objects::retryButton.position.x - Objects::retryButton.width / 2, Objects::retryButton.position.y - Objects::retryButton.height / 2, Objects::retryButton.width, Objects::retryButton.height }))
				{
					Objects::retryButton.text.color = WHITE;

					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
					{
						Externs::retry = true;
						currentScene = GameplayScene::Playing;
					}
				}
				else
				{
					Objects::retryButton.text.color = GRAY;
				}

				if (CheckCollisionPointRec({ Essentials::Objects::cursor.positionX, Essentials::Objects::cursor.positionY }, { Objects::exitButton.position.x - Objects::exitButton.width / 2, Objects::exitButton.position.y - Objects::exitButton.height / 2, Objects::exitButton.width, Objects::exitButton.height }))
				{
					Objects::exitButton.text.color = WHITE;

					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
					{
						currentState = State::Menu;
						Externs::retry = true;
						currentScene = GameplayScene::ReadingRules;
					}
				}
				else
				{
					Objects::exitButton.text.color = GRAY;
				}

				if (Externs::retry)
				{
					Player::Initialization(Playing::Objects::bird1, KEY_W, { static_cast<float>(Externs::screenWidth) / 6.0f, static_cast<float>(Externs::screenHeight) / 2.0f });
					Player::Initialization(Playing::Objects::bird2, KEY_UP, { static_cast<float>(Externs::screenWidth) / 5.0f, static_cast<float>(Externs::screenHeight) / 2.0f });
					Obstacle::Initialization(Playing::Objects::obstacle);

					Externs::retry = false;
				}

				break;

				break;

			default:
				break;
			}
		}

		void Draw()
		{
			if (currentScene != GameplayScene::ReadingRules)
			{
				Assets::Parallax::Draw();

				if (Objects::bird1.isOn)
				{
					Player::Draw(Objects::bird1);
				}

				if (currentPlaystyle == Playstyle::Multiplayer)
				{
					if (Objects::bird2.isOn)
					{
						Player::Draw(Objects::bird2);
					}
				}

				Obstacle::Draw(Objects::obstacle);
			}

			switch (currentScene)
			{

			case Game::Playing::GameplayScene::ReadingRules:

				if (currentPlaystyle == Playstyle::Multiplayer)
				{
					std::string player1Text = "Player1";
					int player1TextFont = 30;
					DrawText(player1Text.c_str(), Externs::screenWidth / 2 - 200 - MeasureText(player1Text.c_str(), player1TextFont) / 2, Externs::screenHeight / 5, player1TextFont, BLACK);

					std::string player2Text = "Player2";
					int player2TextFont = 30;
					DrawText(player2Text.c_str(), Externs::screenWidth / 2 + 200 - MeasureText(player2Text.c_str(), player2TextFont) / 2, Externs::screenHeight / 5, player2TextFont, BLACK);

					std::string player1ControlText = "-W to jump";
					int player1ControlTextFont = 22;
					DrawText(player1ControlText.c_str(), Externs::screenWidth / 2 - 200 - MeasureText(player1ControlText.c_str(), player1ControlTextFont) / 2, Externs::screenHeight / 2 - 50, player1ControlTextFont, BLACK);

					std::string player2ControlText = "-Up arrow to jump";
					int player2ControlTextFont = 22;
					DrawText(player2ControlText.c_str(), Externs::screenWidth / 2 + 200 - MeasureText(player2ControlText.c_str(), player2ControlTextFont) / 2, Externs::screenHeight / 2 - 50, player2ControlTextFont, BLACK);

					std::string continueText = "Press ENTER to continue";
					int continueTextFont = 30;
					DrawText(continueText.c_str(), Externs::screenWidth / 2 - MeasureText(continueText.c_str(), continueTextFont) / 2, Externs::screenHeight / 2 + 150, continueTextFont, BLACK);
				}
				else
				{
					std::string player1Text = "Player";
					int player1TextFont = 30;
					DrawText(player1Text.c_str(), Externs::screenWidth / 2 - MeasureText(player1Text.c_str(), player1TextFont) / 2, Externs::screenHeight / 5, player1TextFont, BLACK);

					std::string player1ControlText = "-W to jump";
					int player1ControlTextFont = 22;
					DrawText(player1ControlText.c_str(), Externs::screenWidth / 2 - MeasureText(player1ControlText.c_str(), player1ControlTextFont) / 2, Externs::screenHeight / 2 - 50, player1ControlTextFont, BLACK);

					std::string continueText = "Press ENTER to continue";
					int continueTextFont = 30;
					DrawText(continueText.c_str(), Externs::screenWidth / 2 - MeasureText(continueText.c_str(), continueTextFont) / 2, Externs::screenHeight / 2 + 150, continueTextFont, BLACK);
				}
				break;

			case Playing::GameplayScene::Playing:
				break;

			case Playing::GameplayScene::Pause:

				break;

			case Playing::GameplayScene::Finished:
				Buttons::Draw(Objects::retryButton);
				Buttons::Draw(Objects::exitButton);
				break;

			default:
				break;
			}
		}

		bool CheckCollisionsCircleRectangle(float circleX, float circleY, float recX, float recY, float width, float height)
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
	}

	namespace Menu
	{
		void Update()
		{
			Essentials::UpdateMousePosition();

			Rectangle singleplayerButt = { Objects::singleplayer.position.x - Objects::singleplayer.width / 2, Objects::singleplayer.position.y - Objects::singleplayer.height / 2, Objects::singleplayer.width, Objects::singleplayer.height };

			if (CheckCollisionPointRec({ Essentials::Objects::cursor.positionX, Essentials::Objects::cursor.positionY }, singleplayerButt))
			{
				Objects::singleplayer.text.color = WHITE;

				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				{
					currentState = State::Playing;
					Playing::currentPlaystyle = Playing::Playstyle::Singleplayer;
				}
			}
			else
			{
				Objects::singleplayer.text.color = GRAY;
			}

			Rectangle multiplayerButt = { Objects::multiplayer.position.x - Objects::multiplayer.width / 2, Objects::multiplayer.position.y - Objects::multiplayer.height / 2, Objects::multiplayer.width, Objects::multiplayer.height };

			if (CheckCollisionPointRec({ Essentials::Objects::cursor.positionX, Essentials::Objects::cursor.positionY }, multiplayerButt))
			{
				Objects::multiplayer.text.color = WHITE;

				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				{
					currentState = State::Playing;
					Playing::currentPlaystyle = Playing::Playstyle::Multiplayer;
				}
			}
			else
			{
				Objects::multiplayer.text.color = GRAY;
			}

			Rectangle creditsButt = { Objects::credits.position.x - Objects::credits.width / 2,Objects::credits.position.y - Objects::credits.height / 2, Objects::credits.width, Objects::credits.height };

			if (CheckCollisionPointRec({ Essentials::Objects::cursor.positionX, Essentials::Objects::cursor.positionY }, creditsButt))
			{
				Objects::credits.text.color = WHITE;
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				{
					currentState = State::Credits;
				}
			}
			else
			{
				Objects::credits.text.color = GRAY;
			}

			Rectangle exitButt = { Objects::exit.position.x - Objects::exit.width / 2, Objects::exit.position.y - Objects::exit.height / 2,Objects::exit.width, Objects::exit.height };

			if (CheckCollisionPointRec({ Essentials::Objects::cursor.positionX, Essentials::Objects::cursor.positionY }, exitButt))
			{
				Objects::exit.text.color = WHITE;
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				{
					currentState = State::Exit;
				}
			}
			else
			{
				Objects::exit.text.color = GRAY;
			}
		}

		void Draw()
		{
			Buttons::Draw(Objects::singleplayer);
			Buttons::Draw(Objects::multiplayer);
			Buttons::Draw(Objects::credits);
			Buttons::Draw(Objects::exit);

			DrawCurrentVer();
		}

		bool CheckCollisionsCircleRectangle(float circleX, float circleY, float recX, float recY, float width, float height)
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

		void DrawCurrentVer()
		{
			Assets::Parallax::version.text = "ver 0.4";
			Assets::Parallax::version.posX = 95.0f;
			Assets::Parallax::version.posY = 98.0f;
			Assets::Parallax::version.color = BLACK;

			Draw::DrawText(Assets::Parallax::version);
		}

		namespace Credits
		{
			void Update()
			{
				Essentials::UpdateMousePosition();

				Rectangle returnButton = { Objects::returnButton.position.x - Objects::returnButton.width / 2, Objects::returnButton.position.y - Objects::returnButton.height / 2, Objects::returnButton.width, Objects::returnButton.height };

				if (CheckCollisionPointRec({ Essentials::Objects::cursor.positionX, Essentials::Objects::cursor.positionY }, returnButton))
				{
					Objects::returnButton.text.color = WHITE;

					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
					{
						currentState = State::Menu;
					}
				}
				else
				{
					Objects::returnButton.text.color = GRAY;
				}
			}

			void Draw()
			{
				Text::Text credits1;
				credits1.text = "Made by Eluney Jazmin Mousseigne";
				credits1.posX = 50;
				credits1.posY = 20;
				credits1.fonstSize = 40;
				credits1.color = BLACK;

				Text::Text credits2;
				credits2.text = "&";
				credits2.posX = 50;
				credits2.posY = 30;
				credits2.fonstSize = 40;
				credits2.color = BLACK;

				Text::Text credits3;
				credits3.text = "Agustin Ezequiel Laure";
				credits3.posX = 50;
				credits3.posY = 40;
				credits3.fonstSize = 40;
				credits3.color = BLACK;

				Draw::DrawText(credits1);
				Draw::DrawText(credits2);
				Draw::DrawText(credits3);
				Buttons::Draw(Objects::returnButton);
				DrawCurrentVer();
			}
		}
	}

	namespace Essentials
	{
		void UpdateMousePosition()
		{
			Objects::cursor.positionX = Utils::PixelsToPercentX(GetMousePosition().x);
			Objects::cursor.positionY = Utils::PixelsToPercentY(GetMousePosition().y);
		};

		void GetDeltaTime()
		{
			Externs::deltaT = GetFrameTime();
		}
	}

	namespace Assets
	{
		namespace Parallax
		{
			void Update()
			{
				scrollingBack -= 1.0f * Externs::deltaT;
				scrollingMid -= 2.0f * Externs::deltaT;
				scrollingFront -= 4.0f * Externs::deltaT;

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

			void Draw()
			{
				Draw::DrawSpriteEx(static_cast<float>(Externs::backgroundBackTextureID), scrollingBack, 0.0f, 200.0f, 100.0f, 0.0f, WHITE);
				Draw::DrawSpriteEx(static_cast<float>(Externs::backgroundBackTextureID), 200.0f + scrollingBack, 0.0f, 200.0f, 100.0f, 0.0f, WHITE);

				Draw::DrawSpriteEx(static_cast<float>(Externs::backgroundMiddleTextureID), scrollingMid, 0.0f, 200.0f, 100.0f, 0.0f, WHITE);
				Draw::DrawSpriteEx(static_cast<float>(Externs::backgroundMiddleTextureID), 200.0f + scrollingMid, 0.0f, 200.0f, 100.0f, 0.0f, WHITE);

				Draw::DrawSpriteEx(static_cast<float>(Externs::backgroundFrontTextureID), scrollingFront, 0.0f, 200.0f, 100.0f, 0.0f, WHITE);
				Draw::DrawSpriteEx(static_cast<float>(Externs::backgroundFrontTextureID), 200.0f + scrollingFront, 0.0f, 200.0f, 100.0f, 0.0f, WHITE);
			}
		}
	}

	void Initialize()
	{
		float buttonWidth = 25.0f;
		float buttonHeight = 8.0f;
		float buttonCenterX = 50.0f;

		//play.text.font = externs::defaultText.font;
		Menu::Objects::singleplayer.text.text = "SINGLEPLAYER";
		Buttons::Initialize(Menu::Objects::singleplayer, buttonWidth, buttonHeight, buttonCenterX, 25.0f);

		Menu::Objects::multiplayer.text.text = "MULTIPLAYER";
		Buttons::Initialize(Menu::Objects::multiplayer, buttonWidth, buttonHeight, buttonCenterX, 35.0f);

		//credits.text.font = externs::defaultText.font;
		Menu::Objects::credits.text.text = "CREDITS";
		Buttons::Initialize(Menu::Objects::credits, buttonWidth, buttonHeight, buttonCenterX, 45.0f);

		//exit.text.font = externs::defaultText.font;
		Menu::Objects::exit.text.text = "EXIT";
		Buttons::Initialize(Menu::Objects::exit, buttonWidth, buttonHeight, buttonCenterX, 55.0f);

		Menu::Credits::Objects::returnButton.text.text = "EXIT";
		Buttons::Initialize(Menu::Credits::Objects::returnButton, buttonWidth, buttonHeight, buttonCenterX, 55.0f);

		Assets::Parallax::tempTexture = LoadTexture(Externs::backgroundFrontTexture.c_str());
		Externs::backgroundFrontTextureID = Assets::Parallax::tempTexture.id;

		Assets::Parallax::tempTexture = LoadTexture(Externs::backgroundMiddleTexture.c_str());
		Externs::backgroundMiddleTextureID = Assets::Parallax::tempTexture.id;

		Assets::Parallax::tempTexture = LoadTexture(Externs::backgroundBackTexture.c_str());
		Externs::backgroundBackTextureID = Assets::Parallax::tempTexture.id;

		Player::Initialization(Playing::Objects::bird1, KEY_W, { static_cast<float>(Externs::screenWidth) / 6.0f, static_cast<float>(Externs::screenHeight) / 2.0f });
		Player::Initialization(Playing::Objects::bird2, KEY_UP, { static_cast<float>(Externs::screenWidth) / 5.0f, static_cast<float>(Externs::screenHeight) / 2.0f });
		Obstacle::Initialization(Playing::Objects::obstacle);

		Playing::Objects::exitButton.text.text = "EXIT";
		Buttons::Initialize(Playing::Objects::exitButton, buttonWidth, buttonHeight, buttonCenterX - 10, 80.0f);

		Playing::Objects::retryButton.text.text = "RETRY";
		Buttons::Initialize(Playing::Objects::retryButton, buttonWidth, buttonHeight, buttonCenterX + 10, 80.0f);
	}

	void GameLoop()
	{
		InitWindow(Externs::screenWidth, Externs::screenHeight, "Flappy Bird");

		Initialize();

		while (!WindowShouldClose() && currentState != State::Exit)
		{
			switch (currentState)
			{
			case State::Menu:

				Menu::Update();

				break;

			case State::Playing:

				Playing::Update();

				break;

			case State::Credits:

				Menu::Credits::Update();

				break;
			default:
				break;
			}

			BeginDrawing();
			ClearBackground(RAYWHITE);

			switch (currentState)
			{
			case State::Menu:

				Menu::Draw();

				break;

			case State::Playing:

				Playing::Draw();

				break;

			case State::Credits:

				Menu::Credits::Draw();

				break;
			default:
				break;
			}

			EndDrawing();
		}

		CloseWindow();
	}
}
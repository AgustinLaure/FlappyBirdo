#include "game_loop.h"
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
	namespace Assets
	{
		static Sound select;
	}

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

	namespace Playing
	{
		namespace Assets
		{
			static Music song;

			static Sound hit;
			static Sound pause;
		}

		namespace Parallax
		{
			static float scrollingBack = 0.0f;
			static float scrollingMid = 0.0f;
			static float scrollingFront = 0.0f;

			static Texture backgroundBackTexture;
			static Texture backgroundFrontTexture;
			static Texture backgroundMiddleTexture;

			static void Reset();
			static void Update();
			static void Draw();
		}

		namespace Objects
		{
			static Player::Bird bird1;
			static Player::Bird bird2;
			static Obstacle::Obstacle obstacle;

			static Buttons::Button retryButton;
			static Buttons::Button exitButton;
			static Buttons::Button resumeButton;
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
		static int score = 0;
		static float timeAlive = 0.0f;

		bool retry = false;
		bool hasLost = false;

		static void Update();
		static void Draw();

		static bool CheckCollisionsCircleRectangle(float circleX, float circleY, float radius, float recX, float recY, float width, float height);
	}

	namespace Menu
	{
		namespace Assets
		{
			static Text::Text version;
			static Music song;
		}

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
		Menu, Playing, Credits, Exit
	};

	static State currentState = State::Menu;

	static void Initialize();
	void UnloadTextures();
	void UnloadSounds();

	namespace Playing
	{
		namespace Parallax
		{
			void Reset()
			{
				scrollingBack = 0.0f;
				scrollingMid = 0.0f;
				scrollingFront = 0.0f;
			}

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
				Draw::DrawSpriteEx(backgroundBackTexture, scrollingBack, 0.0f, 200.0f, 100.0f, 0.0f, WHITE);
				Draw::DrawSpriteEx(backgroundBackTexture, 200.0f + scrollingBack, 0.0f, 200.0f, 100.0f, 0.0f, WHITE);

				Draw::DrawSpriteEx(backgroundMiddleTexture, scrollingMid, 0.0f, 200.0f, 100.0f, 0.0f, WHITE);
				Draw::DrawSpriteEx(backgroundMiddleTexture, 200.0f + scrollingMid, 0.0f, 200.0f, 100.0f, 0.0f, WHITE);

				Draw::DrawSpriteEx(backgroundFrontTexture, scrollingFront, 0.0f, 200.0f, 100.0f, 0.0f, WHITE);
				Draw::DrawSpriteEx(backgroundFrontTexture, 200.0f + scrollingFront, 0.0f, 200.0f, 100.0f, 0.0f, WHITE);
			}
		}

		void Update()
		{
			if (currentScene != GameplayScene::ReadingRules)
			{
				UpdateMusicStream(Assets::song);
			}
			else
			{
				UpdateMusicStream(Menu::Assets::song);
			}

			switch (currentScene)
			{
			case GameplayScene::ReadingRules:
				if (IsKeyPressed(KEY_ENTER))
				{
					currentScene = GameplayScene::Playing;
				}
				break;

			case GameplayScene::Playing:

				timeAlive += Externs::deltaT;

				if (!Objects::obstacle.passed)
				{
					if (currentPlaystyle == Playstyle::Singleplayer)
					{
						if (Objects::obstacle.bottom.x < Objects::bird1.position.x)
						{
							score += 1;
							Objects::obstacle.passed = true;
						}
					}
					else
					{
						if (Objects::obstacle.bottom.x < Objects::bird2.position.x)
						{
							score += 1;
							Objects::obstacle.passed = true;
						}
					}
				}

				if (IsKeyPressed(KEY_P))
				{
					PlaySound(Assets::pause);
					currentScene = GameplayScene::Pause;
				}

				Parallax::Update();

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
				else
				{
					Objects::bird2.isOn = false;
				}

				Obstacle::Update(Objects::obstacle);

				if (Objects::bird1.isOn)
				{
					if (CheckCollisionsCircleRectangle(Objects::bird1.position.x, Objects::bird1.position.y, Player::birdRadius, Objects::obstacle.bottom.x,
						Objects::obstacle.bottom.y, Objects::obstacle.width, Objects::obstacle.height) || CheckCollisionsCircleRectangle
						(Objects::bird1.position.x, Objects::bird1.position.y, Player::birdRadius, Objects::obstacle.top.x, Objects::obstacle.top.y, Objects::obstacle.width, Objects::obstacle.height))
					{
						PlaySound(Assets::hit);
						Objects::bird1.isOn = false;
					}
				}

				if (Objects::bird2.isOn)
				{
					if (CheckCollisionsCircleRectangle(Objects::bird2.position.x, Objects::bird2.position.y, Player::birdRadius, Objects::obstacle.bottom.x,
						Objects::obstacle.bottom.y, Objects::obstacle.width, Objects::obstacle.height) || CheckCollisionsCircleRectangle
						(Objects::bird2.position.x, Objects::bird2.position.y, Player::birdRadius, Objects::obstacle.top.x, Objects::obstacle.top.y, Objects::obstacle.width, Objects::obstacle.height))
					{
						PlaySound(Assets::hit);
						Objects::bird2.isOn = false;
					}
				}

				if (!Objects::bird1.isOn)
				{
					if (currentPlaystyle == Playstyle::Multiplayer)
					{
						if (!Objects::bird2.isOn)
						{
							hasLost = true;
						}
					}
					else
					{
						hasLost = true;
					}
				}

				if (hasLost)
				{
					currentScene = GameplayScene::Finished;
				}

				break;

			case GameplayScene::Pause:

				Essentials::UpdateMousePosition();

				if (CheckCollisionPointRec({ Essentials::Objects::cursor.positionX, Essentials::Objects::cursor.positionY }, { Objects::exitButton.position.x - Objects::exitButton.width / 2, Objects::exitButton.position.y - Objects::exitButton.height / 2, Objects::exitButton.width, Objects::exitButton.height }))
				{
					Objects::exitButton.text.color = WHITE;

					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
					{
						retry = true;
						currentScene = GameplayScene::ReadingRules;
						currentState = State::Menu;
					}
				}
				else
				{
					Objects::exitButton.text.color = GRAY;
				}

				if (CheckCollisionPointRec({ Essentials::Objects::cursor.positionX, Essentials::Objects::cursor.positionY }, { Objects::resumeButton.position.x - Objects::resumeButton.width / 2, Objects::resumeButton.position.y - Objects::resumeButton.height / 2, Objects::resumeButton.width, Objects::resumeButton.height }))
				{
					Objects::resumeButton.text.color = WHITE;

					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
					{
						PlaySound(Game::Assets::select);
						currentScene = GameplayScene::Playing;
					}
				}
				else
				{
					Objects::resumeButton.text.color = GRAY;
				}

				break;
			case GameplayScene::Finished:

				Essentials::UpdateMousePosition();

				if (CheckCollisionPointRec({ Essentials::Objects::cursor.positionX, Essentials::Objects::cursor.positionY }, { Objects::retryButton.position.x - Objects::retryButton.width / 2, Objects::retryButton.position.y - Objects::retryButton.height / 2, Objects::retryButton.width, Objects::retryButton.height }))
				{
					Objects::retryButton.text.color = WHITE;

					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
					{
						PlaySound(Game::Assets::select);
						retry = true;
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
						retry = true;
						currentScene = GameplayScene::ReadingRules;
					}
				}
				else
				{
					Objects::exitButton.text.color = GRAY;
				}

				break;

			default:
				break;
			}

			if (retry)
			{
				Player::Initialization(Playing::Objects::bird1, WHITE, KEY_W, { static_cast<float>(Externs::screenWidth) / 6.0f, static_cast<float>(Externs::screenHeight) / 2.0f });
				Player::Initialization(Playing::Objects::bird2, RED, KEY_UP, { static_cast<float>(Externs::screenWidth) / 5.0f, static_cast<float>(Externs::screenHeight) / 2.0f });
				Obstacle::Initialization(Playing::Objects::obstacle);
				Parallax::Reset();
				score = 0;
				timeAlive = 0.0f;

				retry = false;
				hasLost = false;
			}
		}

		void Draw()
		{
			if (currentScene != GameplayScene::ReadingRules)
			{
				Parallax::Draw();

				if (currentPlaystyle == Playstyle::Multiplayer)
				{

					Player::Draw(Objects::bird2);

				}

				Obstacle::Draw(Objects::obstacle);

				Player::Draw(Objects::bird1);

				if (!hasLost)
				{
					DrawText(TextFormat("%d", (score)), Externs::screenWidth / 2 - MeasureText(TextFormat("%d", (score)), 10), Externs::screenHeight / 7, 35, WHITE);
				}
			}

			switch (currentScene)
			{

			case Game::Playing::GameplayScene::ReadingRules:

				if (currentPlaystyle == Playstyle::Multiplayer)
				{
					std::string player1Text = "Player1";
					int player1TextFont = 30;
					DrawText(player1Text.c_str(), Externs::screenWidth / 2 - 200 - MeasureText(player1Text.c_str(), player1TextFont) / 2, Externs::screenHeight / 2 - 30, player1TextFont, BLACK);

					std::string player2Text = "Player2";
					int player2TextFont = 30;
					DrawText(player2Text.c_str(), Externs::screenWidth / 2 + 200 - MeasureText(player2Text.c_str(), player2TextFont) / 2, Externs::screenHeight / 2 - 30, player2TextFont, BLACK);

					std::string player1ControlText = "-W to jump";
					int player1ControlTextFont = 22;
					DrawText(player1ControlText.c_str(), Externs::screenWidth / 2 - 200 - MeasureText(player1ControlText.c_str(), player1ControlTextFont) / 2, Externs::screenHeight / 2 + 100, player1ControlTextFont, BLACK);

					std::string player2ControlText = "-Up arrow to jump";
					int player2ControlTextFont = 22;
					DrawText(player2ControlText.c_str(), Externs::screenWidth / 2 + 200 - MeasureText(player2ControlText.c_str(), player2ControlTextFont) / 2, Externs::screenHeight / 2 + 100, player2ControlTextFont, BLACK);

					std::string continueText = "Press ENTER to continue";
					int continueTextFont = 30;
					DrawText(continueText.c_str(), Externs::screenWidth / 2 - MeasureText(continueText.c_str(), continueTextFont) / 2, Externs::screenHeight / 2 + 275, continueTextFont, BLACK);

					std::string rulesText1 = "-Flap your way to the end of an endless magical cave";
					std::string rulesText2 = "-Avoid hitting rocks or falling off the screen";

					int rulesText1Font = 30;
					int rulestText2Font = 30;

					DrawText(rulesText1.c_str(), Externs::screenWidth / 2 - MeasureText(rulesText1.c_str(), rulesText1Font) / 2, Externs::screenHeight / 4 - 100, rulesText1Font, BLACK);
					DrawText(rulesText2.c_str(), Externs::screenWidth / 2 - MeasureText(rulesText2.c_str(), rulestText2Font) / 2, Externs::screenHeight / 4, rulestText2Font, BLACK);

				}
				else
				{
					std::string player1Text = "Player";
					int player1TextFont = 39;
					DrawText(player1Text.c_str(), Externs::screenWidth / 2 - MeasureText(player1Text.c_str(), player1TextFont) / 2, Externs::screenHeight / 2 - 50, player1TextFont, BLACK);

					std::string player1ControlText = "-W to jump";
					int player1ControlTextFont = 28;
					DrawText(player1ControlText.c_str(), Externs::screenWidth / 2 - MeasureText(player1ControlText.c_str(), player1ControlTextFont) / 2, Externs::screenHeight / 2 + 85, player1ControlTextFont, BLACK);

					std::string continueText = "Press ENTER to continue";
					int continueTextFont = 30;
					DrawText(continueText.c_str(), Externs::screenWidth / 2 - MeasureText(continueText.c_str(), continueTextFont) / 2, Externs::screenHeight / 2 + 250, continueTextFont, BLACK);

					std::string rulesText1 = "-Flap your way to the end of an endless magical cave";
					std::string rulesText2 = "-Avoid hitting rocks or falling off the screen";

					int rulesText1Font = 30;
					int rulestText2Font = 30;

					DrawText(rulesText1.c_str(), Externs::screenWidth / 2 - MeasureText(rulesText1.c_str(), rulesText1Font) / 2, Externs::screenHeight / 4 - 85, rulesText1Font, BLACK);
					DrawText(rulesText2.c_str(), Externs::screenWidth / 2 - MeasureText(rulesText2.c_str(), rulestText2Font) / 2, Externs::screenHeight / 4, rulestText2Font, BLACK);
				}

				break;

			case Playing::GameplayScene::Playing:
				break;

			case Playing::GameplayScene::Pause:
				DrawText("PAUSED", Externs::screenWidth / 2 - MeasureText("PAUSED", 60) / 2, Externs::screenHeight / 2 - 100, 60, WHITE);
				Buttons::Draw(Objects::resumeButton);
				Buttons::Draw(Objects::exitButton);
				break;

			case Playing::GameplayScene::Finished:
				DrawText("YOU LOST!", Externs::screenWidth / 2 - MeasureText("YOU LOST!", 60) / 2, Externs::screenHeight / 2 - 200, 60, WHITE);
				Buttons::Draw(Objects::retryButton);
				Buttons::Draw(Objects::exitButton);
				DrawText(TextFormat("Score: %d", (score)), Externs::screenWidth / 2 - MeasureText(TextFormat("Score: %d", (score)), 10), Externs::screenHeight / 2, 20, WHITE);
				DrawText(TextFormat("Seconds alive: %d", static_cast<int>(timeAlive)), Externs::screenWidth / 2 - MeasureText(TextFormat("Seconds alive: %d", static_cast<int>(timeAlive)), 10), Externs::screenHeight / 2 + 35, 20, WHITE);
				break;

			default:
				break;
			}
		}

		bool CheckCollisionsCircleRectangle(float circleX, float circleY, float radius, float recX, float recY, float width, float height)
		{
			float closestX = std::max(recX, std::min(circleX, recX + width));
			float closestY = std::max(recY, std::min(circleY, recY + height));

			float distanceX = circleX - closestX;
			float distanceY = circleY - closestY;

			float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

			if (distanceSquared < (radius * radius))
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
					PlaySound(Game::Assets::select);
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
					PlaySound(Game::Assets::select);
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
					PlaySound(Game::Assets::select);
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
					PlaySound(Game::Assets::select);
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
			DrawText("BAT ADVENTURE",Externs::screenWidth/2- MeasureText("BAT ADVENTURE", 50)/2, Externs::screenHeight / 2-240, 50, BLACK);
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
			Assets::version.text = "ver 0.4";
			Assets::version.posX = 95.0f;
			Assets::version.posY = 98.0f;
			Assets::version.color = BLACK;

			Draw::DrawText(Assets::version);
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
						PlaySound(Game::Assets::select);
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
				credits1.text = "Programmed by:           Eluney Jazmin Mousseigne";
				credits1.posX = 25;
				credits1.posY = 20;
				credits1.fontSize = 24;
				credits1.color = BLACK;

				Text::Text credits2;
				credits2.text = "&";
				credits2.posX = 50;
				credits2.posY = 20;
				credits2.fontSize = 30;
				credits2.color = BLACK;

				Text::Text credits3;
				credits3.text = "Agustin Ezequiel Laure";
				credits3.posX = 64;
				credits3.posY = 20;
				credits3.fontSize = 24;
				credits3.color = BLACK;

				Text::Text credits4;
				credits4.text = "Assets by: ";
				credits4.posX = 12;
				credits4.posY = 40;
				credits4.fontSize = 24;
				credits4.color = BLACK;

				Text::Text credits5;
				credits5.text = "-Vittorio Dolce";
				credits5.posX = 37;
				credits5.posY = 40;
				credits5.fontSize = 30;
				credits5.color = BLACK;

				Text::Text credits6;
				credits6.text = "-Pixabay - Sound effects";
				credits6.posX = 70;
				credits6.posY = 40;
				credits6.fontSize = 30;
				credits6.color = BLACK;

				Text::Text credits7;
				credits7.text = "-Ateliermagicae - Itchio";
				credits7.posX = 38;
				credits7.posY = 60;
				credits7.fontSize = 30;
				credits7.color = BLACK;

				Text::Text credits8;
				credits8.text = "-Duxmusic - Itchio";
				credits8.posX = 69;
				credits8.posY = 60;
				credits8.fontSize = 30;
				credits8.color = BLACK;

				Text::Text credits9;
				credits9.text = "-Kashdanmusic - Itchio";
				credits9.posX = 51;
				credits9.posY = 77;
				credits9.fontSize = 30;
				credits9.color = BLACK;



				Draw::DrawText(credits1);
				Draw::DrawText(credits3);
				Draw::DrawText(credits2);
				Draw::DrawText(credits4);
				Draw::DrawText(credits5);
				Draw::DrawText(credits6);
				Draw::DrawText(credits7);
				Draw::DrawText(credits8);
				Draw::DrawText(credits9);
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

	void Initialize()
	{
		float buttonWidth = 25.0f;
		float buttonHeight = 10.0f;
		float buttonCenterX = 50.0f;

		//play.text.font = externs::defaultText.font;
		Menu::Objects::singleplayer.text.text = "SINGLEPLAYER";
		Buttons::Initialize(Menu::Objects::singleplayer, buttonWidth, buttonHeight, buttonCenterX ,  25.0f + 20);

		Menu::Objects::multiplayer.text.text = "MULTIPLAYER";
		Buttons::Initialize(Menu::Objects::multiplayer, buttonWidth, buttonHeight , buttonCenterX , 35.0f + 20);

		//credits.text.font = externs::defaultText.font;
		Menu::Objects::credits.text.text = "CREDITS";
		Buttons::Initialize(Menu::Objects::credits, buttonWidth, buttonHeight, buttonCenterX , 45.0f + 20);

		//exit.text.font = externs::defaultText.font;
		Menu::Objects::exit.text.text = "EXIT";
		Buttons::Initialize(Menu::Objects::exit, buttonWidth, buttonHeight , buttonCenterX , 55.0f + 20);

		Menu::Credits::Objects::returnButton.text.text = "EXIT";
		Buttons::Initialize(Menu::Credits::Objects::returnButton, buttonWidth, buttonHeight, buttonCenterX+40, 55.0f+25);

		Player::Initialization(Playing::Objects::bird1, WHITE, KEY_W, { static_cast<float>(Externs::screenWidth) / 6.0f, static_cast<float>(Externs::screenHeight) / 2.0f });
		Player::Initialization(Playing::Objects::bird2, RED, KEY_UP, { static_cast<float>(Externs::screenWidth) / 5.0f, static_cast<float>(Externs::screenHeight) / 2.0f });
		Obstacle::Initialization(Playing::Objects::obstacle);

		Playing::Objects::exitButton.text.text = "EXIT";
		Buttons::Initialize(Playing::Objects::exitButton, buttonWidth, buttonHeight, buttonCenterX - 14, 80.0f);

		Playing::Objects::retryButton.text.text = "RETRY";
		Buttons::Initialize(Playing::Objects::retryButton, buttonWidth, buttonHeight , buttonCenterX + 14, 80.0f);

		Playing::Objects::resumeButton.text.text = "RESUME";
		Buttons::Initialize(Playing::Objects::resumeButton, buttonWidth, buttonHeight, buttonCenterX + 10, 80.0f);

		Playing::Parallax::backgroundFrontTexture = LoadTexture(Externs::backgroundFrontTexture.c_str());

		Playing::Parallax::backgroundMiddleTexture = LoadTexture(Externs::backgroundMiddleTexture.c_str());

		Playing::Parallax::backgroundBackTexture = LoadTexture(Externs::backgroundBackTexture.c_str());

		Playing::Assets::song = LoadMusicStream(Externs::playingSong.c_str());
		PlayMusicStream(Playing::Assets::song);

		SetMusicVolume(Playing::Assets::song, 0.5);

		Menu::Assets::song = LoadMusicStream(Externs::menuSong.c_str());
		PlayMusicStream(Menu::Assets::song);

		SetMusicVolume(Menu::Assets::song, 0.5);

		Assets::select = LoadSound(Externs::selectSound.c_str());
		Playing::Assets::hit = LoadSound(Externs::birdHitSound.c_str());
		Playing::Assets::pause = LoadSound(Externs::pauseSound.c_str());

	}

	void UnloadTextures()
	{
		UnloadTexture(Playing::Parallax::backgroundFrontTexture);
		UnloadTexture(Playing::Parallax::backgroundMiddleTexture);
		UnloadTexture(Playing::Parallax::backgroundBackTexture);
	}

	void UnloadSounds()
	{
		UnloadMusicStream(Playing::Assets::song);
		Player::Deinit();
		UnloadSound(Playing::Assets::pause);
		UnloadSound(Playing::Assets::hit);
		UnloadSound(Assets::select);
	}

	void GameLoop()
	{
		InitWindow(Externs::screenWidth, Externs::screenHeight, "Flappy Bird");
		InitAudioDevice();

		Initialize();

		while (!WindowShouldClose() && currentState != State::Exit)
		{
			switch (currentState)
			{
			case State::Menu:

				UpdateMusicStream(Menu::Assets::song);
				Menu::Update();

				break;

			case State::Playing:

				Playing::Update();

				break;

			case State::Credits:

				UpdateMusicStream(Menu::Assets::song);
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

		UnloadSounds();
		UnloadTextures();

		CloseAudioDevice();
		CloseWindow();
	}
}
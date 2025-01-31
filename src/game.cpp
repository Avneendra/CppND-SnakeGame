#include "game.h"
#include <iostream>
#include "SDL.h"

Game::Game(std::size_t grid_width, std::size_t grid_height, std::size_t screen_width, std::size_t screen_height, std::string &playerName)
    : snake(grid_width, grid_height),
      gridWidth(grid_width),
      gridHeight(grid_height),
      playerInfo(playerName),
      engine(dev()),
      random_w(0, static_cast<int>(screen_width/grid_width)),
      random_h(0, static_cast<int>(screen_height/grid_height)) {

  PlaceFood();
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;
  bool returnKey = false;

  std::string msg = "Hello, " + playerInfo.GetPlayerName() + "! Press Enter to start game, Esc to quit.";
  DisplayScreen(controller, renderer, msg, running, returnKey, target_frame_duration);

  while (running) 
  {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    Update();
    renderer.Render(snake, food);

    if(!running || !snake.alive)
    {
      std::string name = "";
      int highScore = 0;
      playerInfo.GetHighScoreAndName(highScore, name);

      msg = "Your score is " + std::to_string(GetScore()) + ".High score is " + std::to_string(highScore) + 
            " by " + name + ". Press Enter to play again, Esc to quit.";
      returnKey = false;
      running = true;
      DisplayScreen(controller, renderer, msg, running, returnKey, target_frame_duration, 15);
      
      // Reset the attributes in Snake and playerInfo class
      snake = Snake(gridWidth, gridHeight);
      name = playerInfo.GetPlayerName();
      playerInfo = PlayersInfo(name);
    }

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(GetScore(), frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake.SnakeCell(x, y)) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}

void Game::DisplayScreen(Controller const &controller, Renderer &renderer,
                         std::string &message,  bool &running, bool &returnKey,
                         std::size_t target_frame_duration, int fontSize)
{
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;

  while(!returnKey && running)
  {

    frame_start = SDL_GetTicks();
    controller.HandleInput(returnKey, running);
    renderer.Render(message, fontSize);
    frame_end = SDL_GetTicks();

    frame_duration = frame_end - frame_start;

    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::Update() {
  if (!snake.alive) return;

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    playerInfo.IncrementPlayerScore();
    PlaceFood();
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.02;
  }
}

unsigned int Game::GetScore() const { return playerInfo.GetPlayerScore(); }
int Game::GetSize() const  { return  snake.size; }
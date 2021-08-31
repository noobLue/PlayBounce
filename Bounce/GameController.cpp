#include "GameController.h"
#include <chrono>
#include <iostream>

void GameController::loop()
{
	int tick_millis = 8;

	auto started = std::chrono::high_resolution_clock::now();
	while (running) 
	{
		auto now = std::chrono::high_resolution_clock::now();
		int elapsed_t = std::chrono::duration_cast<std::chrono::milliseconds>(now - started).count();

		int delta_t = tick_millis * current_tick - elapsed_t;
		if (delta_t > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(delta_t));
		}
		current_tick++;

		// Update ball state
		if (ball.pos.y < 0) {
			ball.dir.y *= -1;

			counter.hit_wall++;
		}
		else if (ball.pos.y >= game_size.y) {
			ball.dir.y *= -1;

			counter.hit_wall++;
		}

		if (ball.pos.x < 0) {
			player2.score++;
			ball.pos = BALL_STARTING_SPOT;
			ball.reset_dir({ 1.0f, 1.0f });

			counter.hit_goal++;
		}
		else if (ball.pos.x >= game_size.x) {
			player1.score++;
			ball.pos = BALL_STARTING_SPOT;
			ball.reset_dir({ -1.0f, 1.0f });

			counter.hit_goal++;
		}

		// Update paddle positions
		// TODO: move key detection outside of this class
		if (IsKeyDown(KEY_W)) player1.pos.y -= 1.0f;
		if (IsKeyDown(KEY_S)) player1.pos.y += 1.0f;

		if (IsKeyDown(KEY_I)) player2.pos.y -= 1.0f;
		if (IsKeyDown(KEY_K)) player2.pos.y += 1.0f;

		if (player1.Contains(ball.pos)) 
		{
			float y_dist = abs((float)player1.pos.y - ball.pos.y); // distance from paddle center
			float y_relative = y_dist / (player1.size.y / 2.0f) * ball.speed;

			ball.dir.y = (ball.dir.y < 0 ? -1 : 1) * y_relative;
			ball.dir.x = sqrt(pow(ball.speed, 2) - pow(y_relative, 2));

			ball.speed += 0.1f;

			counter.hit_player++;
		}

		if (player2.Contains(ball.pos)) 
		{
			float y_dist = abs((float)player2.pos.y - ball.pos.y); // distance from paddle center
			float y_relative = y_dist / (player2.size.y / 2.0f) * ball.speed;

			ball.dir.y = ( ball.dir.y < 0 ? -1 : 1) * y_relative;
			ball.dir.x = -sqrt(pow(ball.speed, 2) - pow(y_relative, 2));

			ball.speed += 0.1f;

			counter.hit_player++;
		}


		// Apply movement to ball
		ball.pos = ball.pos + ball.dir;
	}
}

GameController::GameController(Vector2d game_size)
{
	this->game_size = game_size;
	BALL_STARTING_SPOT = { game_size.x / 2.0f, game_size.y / 2.0f };

	Vector2d paddle_size = Vector2d(10, game_size.y / 7);

	player1 = Paddle(Vector2d(50, game_size.y / 2), paddle_size);
	player2 = Paddle(Vector2d(game_size.x - 50, game_size.y / 2), paddle_size);
	ball = Ball(BALL_STARTING_SPOT);
}

void GameController::start()
{
	game_loop = std::thread(&GameController::loop, this);
}

void GameController::stop()
{
	running = false;
	game_loop.join();
}

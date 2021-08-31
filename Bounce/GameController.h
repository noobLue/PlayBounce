#pragma once
#include <thread>
#include "Vector2d.h"


// Refactor to a rectangle class
struct Paddle {
	Vector2d pos;
	Vector2d size;

	int score = 0;
	Paddle() {}

	Paddle(Vector2d pos, Vector2d size) {
		this->pos = pos;
		this->size = size;
	}

	// could be rectangle method
	bool Contains(Vector2 ball)
	{
		return ball.x >= pos.x - size.x / 2
			&& ball.y >= pos.y - size.y / 2
			&& ball.x < pos.x + size.x / 2
			&& ball.y < pos.y + size.y / 2;
	}
};

struct Ball {
	float speed = 1.0f;
	Vector2 pos;
	Vector2 dir;

	Ball() { pos = { 0,0 }; dir = { 0,0 }; }

	Ball(Vector2 pos) {
		this->pos = pos;
		reset_dir({1.0f, 1.0f});
		//dir = { 1, 1 };
		//speed = sqrt(pow(dir.x, 2) + pow(dir.y, 2));
	}

	void reset_dir(Vector2 dir) {

		this->dir = dir;
		this->speed = sqrt(pow(this->dir.x, 2) + pow(this->dir.y, 2));
	}
};

struct AudioCounter {
	int hit_wall = 0;
	int hit_player = 0;
	int hit_goal = 0;
};


// TODO: add mutex for data access
class GameController
{
	Vector2 BALL_STARTING_SPOT;

	std::thread game_loop;
	void loop();
public:
	Vector2d game_size;
	Paddle player1;
	Paddle player2;
	Ball ball;

	AudioCounter counter;
	
	bool running = true;
	int current_tick = 0;

	GameController(Vector2d game_size);

	void start();
	void stop();
};


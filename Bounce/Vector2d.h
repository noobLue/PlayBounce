#pragma once
#include "raymath.h"

// TODO: rename file to raymathext

inline Vector2 operator+(const Vector2& lhs, const Vector2& rhs) {
	Vector2 val;
	val.x = lhs.x + rhs.x;
	val.y = lhs.y + rhs.y;
	return val;
}

struct Vector2d {
	int x;
	int y;

	Vector2d() { x = 0; y = 0; }

	Vector2d(int x, int y) {
		this->x = x;
		this->y = y;
	}

	Vector2 as_vector2() {
		Vector2 vec;
		vec.x = x;
		vec.y = y;

		return vec;
	}

	Vector2d& operator+=(const Vector2d& rhs) {
		this->x += rhs.x;
		this->y += rhs.y;
		return *this;
	}
};


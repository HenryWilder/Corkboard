#pragma once
#include <raylib.h>
#include <vector>

constexpr Color threadWhite = {255,255,255,255};
constexpr Color threadRed = {245, 66, 66,255};
constexpr Color threadOrange = {245, 126, 66, 255};
constexpr Color threadYellow = {245, 215, 66,255};
constexpr Color threadGreen = {70, 184, 119,255};
constexpr Color threadLightBlue = {66, 221, 245,255};
constexpr Color threadBlue = {55, 58, 166,255};
constexpr Color threadPurple = {112, 45, 145,255};
constexpr Color threadPink = {238, 120, 240,255};

class Notecard;

class Thread
{
public:
	Thread(Color color, Notecard* start, Notecard* end) : color(color), start(start), end(end) {}

	constexpr static float thickness = 4.0f;

	Color color;
	Notecard* start;
	Notecard* end;

	Vector2 StartPosition() const;
	Vector2 EndPosition() const;

	void Draw() const;
	bool IsHovered(Vector2) const;
};

extern std::vector<Thread*> g_threads;

void CreateThread(Color color, Notecard* start, Notecard* end);

void DestroyThread(Thread* thread, Notecard* ignoring = nullptr);

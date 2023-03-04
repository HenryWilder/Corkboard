#pragma once
#include <raylib.h>
#include <string>
#include <unordered_set>

constexpr Color cardstockWhite = {240,245,245,255};
constexpr Color cardstockRed = {255,170,170,255};
constexpr Color cardstockOrange = {255,200,170,255};
constexpr Color cardstockYellow = {255,240,170,255};
constexpr Color cardstockGreen = {190,255,170,255};
constexpr Color cardstockLightBlue = {170,255,250,255};
constexpr Color cardstockBlue = {170,190,255,255};
constexpr Color cardstockLavender = {200,170,255,255};
constexpr Color cardstockPink = {245,210,255,255};

// Todo: find better colors.
constexpr Color cardLineRed = RED;
constexpr Color cardLineBlue = SKYBLUE;

class Thread;

class Notecard
{
public:
	Notecard(Vector2 position, Color color) : position(position), color(color) {}
	Notecard(Vector2 position, Color color, const std::string& title, const std::string& content) : position(position), color(color), title(title), content(content) {}

	constexpr static Vector2 size = { 5 * 40, 3 * 40 };
	constexpr static float pinRadius = 6.0f;
	constexpr static Vector2 pinOffset = { size.x / 2, pinRadius + 1 }; // The position of the pin

	Vector2 position; // The position of the card's top-left corner
	Color color;

	constexpr static float titleSize = 20.0f;
	std::string title;

	constexpr static float contentSize = 10.0f;
	std::string content;

	std::unordered_set<Thread*> threads; // Set of connected threads

	void AddThreadConnection(Thread*);
	void RemoveThreadConnection(Thread*);

	Vector2 PinPosition() const;

	// The card is on a layer below the threads
	void DrawCard() const;
	void DrawCardGhost() const;

	// The pin is on a layer above the threads
	void DrawPin() const;

	bool IsCardHovered(Vector2 point) const;
	bool IsPinHovered(Vector2 point) const;

	Rectangle GetCardRectangle() const;
};

extern std::vector<Notecard*> g_cards;

void CreateCard(Vector2 position, Color color);

void DestroyCard(Notecard* card);

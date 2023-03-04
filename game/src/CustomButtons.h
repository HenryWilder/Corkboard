#pragma once
#include <variant>
#include <raylib.h>

extern Color cardColor;
extern Color threadColor;

enum class ButtonState { Inactive, Active, Disabled };

struct GenericButtonData
{
	GenericButtonData(int x, int y, ButtonState startingState = ButtonState::Inactive)
		: x(x), y(y), state(startingState) {}

	static constexpr int width = 30;
	static constexpr int height = 30;
	int x;
	int y;
	ButtonState state;

	void Draw() const;
	bool IsHovered(Vector2 point) const;

	Rectangle GetRectangle() const;
};

class ThreadColorButton
{
public:
	ThreadColorButton(Color color) : genPtr(), color(color) {}

	GenericButtonData* genPtr;
	Color color;

	void DrawIcon() const;
	void OnClick();
};

class CardColorButton
{
public:
	CardColorButton(Color color) : genPtr(), color(color) {}

	GenericButtonData* genPtr;
	Color color;

	void DrawIcon() const;
	void OnClick();
};

class SaveButton
{
public:
	SaveButton() : genPtr() {}

	GenericButtonData* genPtr;

	void DrawIcon() const;
	void OnClick();
};


class LoadButton
{
public:
	LoadButton() : genPtr() {}

	GenericButtonData* genPtr;

	void DrawIcon() const;
	void OnClick();
};

using ButtonVariant = std::variant<ThreadColorButton, CardColorButton, SaveButton, LoadButton>;

class ButtonWrapper
{
public:
	GenericButtonData genericData;
	ButtonVariant specialistData;

	ButtonWrapper(int x, int y, ThreadColorButton&& threadColorButton)
		: genericData(x,y), specialistData(threadColorButton) { std::get<ThreadColorButton>(specialistData).genPtr = &genericData; }
	ButtonWrapper(int x, int y, CardColorButton&& cardColorButton)
		: genericData(x,y), specialistData(cardColorButton) { std::get<CardColorButton>(specialistData).genPtr = &genericData; }
	ButtonWrapper(int x, int y, SaveButton&& cardColorButton)
		: genericData(x,y), specialistData(cardColorButton) { std::get<SaveButton>(specialistData).genPtr = &genericData; }
	ButtonWrapper(int x, int y, LoadButton&& cardColorButton)
		: genericData(x,y), specialistData(cardColorButton) { std::get<LoadButton>(specialistData).genPtr = &genericData; }

	void Draw() const;
	bool IsHovered(Vector2 point) const;
	void OnClick();
	Rectangle GetRectangle() const;
};

extern GenericButtonData* activeThreadColorButton;
extern GenericButtonData* activeCardColorButton;

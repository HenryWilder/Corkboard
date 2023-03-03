#pragma once
#include <raylib.h>

extern Color cardColor;
extern Color threadColor;

class CBButton
{
public:
	CBButton(int x, int y) : position{(float)x,(float)y} {}

	static constexpr Vector2 size = {30,30};
	Vector2 position;

	virtual void Draw() const;
	bool IsHovered(Vector2 point) const;
	virtual void OnClick() const = 0;

	Rectangle GetRectangle() const;
};

class ThreadColorButton : public CBButton
{
public:
	ThreadColorButton(int x, int y, Color color) : CBButton(x,y), color(color) {}
	ThreadColorButton(float x, float y, Color color) : CBButton((int)x,(int)y), color(color) {}

	Color color;

	void Draw() const override;
	void OnClick() const override;
};

class CardColorButton : public CBButton
{
public:
	CardColorButton(int x, int y, Color color) : CBButton(x,y), color(color) {}

	Color color;

	void Draw() const override;
	void OnClick() const override;
};

#include "CBButton.h"
#include "VectorMath.h"

Color cardColor;
Color threadColor;

void CBButton::Draw() const
{
	DrawRectangleV(position, size, GRAY);

	DrawRectangleLines((int)position.x, (int)position.y, (int)size.x, (int)size.y, DARKGRAY);
}

bool CBButton::IsHovered(Vector2 point) const
{
	return CheckCollisionPointRecV(point, position, size);
}

Rectangle CBButton::GetRectangle() const
{
	return Rectangle{ position.x, position.y, size.x, size.y };
}

void ThreadColorButton::Draw() const
{
	CBButton::Draw();

	int left = (int)position.x + 5;
	int width = (int)size.x - 10;

	DrawRectangle(left, (int)position.y + (int)size.y / 2 - 1, width, 2, color);
}

void ThreadColorButton::OnClick() const
{
	threadColor = color;
}

void CardColorButton::Draw() const
{
	CBButton::Draw();

	int left = (int)position.x + 5;
	int width = (int)size.x - 10;

	DrawRectangle(left, (int)position.y + 8, width, (int)size.y - 16, color);
}

void CardColorButton::OnClick() const
{
	cardColor = color;
}

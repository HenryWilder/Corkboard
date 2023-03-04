#include <variant>
#include "CustomButtons.h"
#include "VectorMath.h"

Color cardColor;
Color threadColor;
GenericButtonData* activeThreadColorButton;
GenericButtonData* activeCardColorButton;

#define TOGGLE_BUTTON_GROUP(groupActiveElement) \
	groupActiveElement->state = ButtonState::Inactive; \
	groupActiveElement = genPtr; \
	genPtr->state = ButtonState::Active

void GenericButtonData::Draw() const
{
	DrawRectangle(x, y, width, height, state == ButtonState::Disabled ? DARKGRAY : GRAY);
	if (state == ButtonState::Active)
		DrawRectangleLinesEx(GetRectangle(), 2, DARKGRAY);
}

bool GenericButtonData::IsHovered(Vector2 point) const
{
	int px = (int)point.x;
	int py = (int)point.y;
	bool result = 
		px >= x && px < x + width &&
		py >= y && py < y + height;
	return result;
}

Rectangle GenericButtonData::GetRectangle() const
{
	Rectangle rect = {
		(float)x,
		(float)y,
		(float)width,
		(float)height
	};

	return rect;
}

void ThreadColorButton::DrawIcon() const
{
	int left = genPtr->x + 5;
	int top = genPtr->y + genPtr->y / 2 - 1;
	int width = genPtr->width - 10;
	int height = 2;

	DrawRectangle(left, top, width, height, color);
}

void ThreadColorButton::OnClick()
{
	threadColor = color;
	TOGGLE_BUTTON_GROUP(activeThreadColorButton);
}

void CardColorButton::DrawIcon() const
{
	int left = genPtr->x + 5;
	int top = genPtr->y + 8;
	int width = genPtr->width - 10;
	int height = genPtr->height - 16;

	DrawRectangle(left, top, width, height, color);
}

void CardColorButton::OnClick()
{
	cardColor = color;
	TOGGLE_BUTTON_GROUP(activeCardColorButton);
}

void SaveButton::DrawIcon() const
{
	// todo
}

void SaveButton::OnClick()
{
	// todo
}

void LoadButton::DrawIcon() const
{
	// todo
}

void LoadButton::OnClick()
{
	// todo
}

// Make sure to update this with new alternatives
#define BUTTON_WRAPPER_SPECIALIZE(x) \
	if (std::holds_alternative<ThreadColorButton>(specialistData)) std::get<ThreadColorButton>(specialistData).x; \
	else if (std::holds_alternative<CardColorButton>(specialistData)) std::get<CardColorButton>(specialistData).x; \
	else if (std::holds_alternative<SaveButton>(specialistData)) std::get<SaveButton>(specialistData).x; \
	else if (std::holds_alternative<LoadButton>(specialistData)) std::get<LoadButton>(specialistData).x

void ButtonWrapper::Draw() const
{
	genericData.Draw();
	BUTTON_WRAPPER_SPECIALIZE(DrawIcon());
}

bool ButtonWrapper::IsHovered(Vector2 point) const
{
	return genericData.IsHovered(point);
}

void ButtonWrapper::OnClick()
{
	BUTTON_WRAPPER_SPECIALIZE(OnClick());
}

Rectangle ButtonWrapper::GetRectangle() const
{
	return genericData.GetRectangle();
}

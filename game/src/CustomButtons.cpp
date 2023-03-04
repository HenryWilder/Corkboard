#include <variant>
#include "CustomButtons.h"
#include "Thread.h"
#include "Notecard.h"
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

std::vector<ButtonWrapper*> g_buttons = {
	// Card colors
	new ButtonWrapper(GenericButtonData::width * 0, 0, CardColorButton(cardstockWhite)),
	new ButtonWrapper(GenericButtonData::width * 1, 0, CardColorButton(cardstockRed)),
	new ButtonWrapper(GenericButtonData::width * 2, 0, CardColorButton(cardstockOrange)),
	new ButtonWrapper(GenericButtonData::width * 3, 0, CardColorButton(cardstockYellow)),
	new ButtonWrapper(GenericButtonData::width * 4, 0, CardColorButton(cardstockGreen)),
	new ButtonWrapper(GenericButtonData::width * 5, 0, CardColorButton(cardstockLightBlue)),
	new ButtonWrapper(GenericButtonData::width * 6, 0, CardColorButton(cardstockBlue)),
	new ButtonWrapper(GenericButtonData::width * 7, 0, CardColorButton(cardstockLavender)),
	new ButtonWrapper(GenericButtonData::width * 8, 0, CardColorButton(cardstockPink)),

	// Thread colors
	// Todo: Make specialized colors
	new ButtonWrapper(GenericButtonData::width * 0, GenericButtonData::height, ThreadColorButton(threadWhite)),
	new ButtonWrapper(GenericButtonData::width * 1, GenericButtonData::height, ThreadColorButton(threadRed)),
	new ButtonWrapper(GenericButtonData::width * 2, GenericButtonData::height, ThreadColorButton(threadOrange)),
	new ButtonWrapper(GenericButtonData::width * 3, GenericButtonData::height, ThreadColorButton(threadYellow)),
	new ButtonWrapper(GenericButtonData::width * 4, GenericButtonData::height, ThreadColorButton(threadGreen)),
	new ButtonWrapper(GenericButtonData::width * 5, GenericButtonData::height, ThreadColorButton(threadLightBlue)),
	new ButtonWrapper(GenericButtonData::width * 6, GenericButtonData::height, ThreadColorButton(threadBlue)),
	new ButtonWrapper(GenericButtonData::width * 7, GenericButtonData::height, ThreadColorButton(threadPurple)),
	new ButtonWrapper(GenericButtonData::width * 8, GenericButtonData::height, ThreadColorButton(threadPink)),
};

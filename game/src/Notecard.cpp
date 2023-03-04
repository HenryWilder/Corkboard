#include "Notecard.h"
#include "Thread.h"
#include <raymath.h>
#include <algorithm>

void Notecard::AddThreadConnection(Thread* thread)
{
	threads.insert(thread);
}

void Notecard::RemoveThreadConnection(Thread* thread)
{
	threads.erase(thread);
}

Vector2 Notecard::PinPosition() const
{
	return Vector2Add(position, pinOffset);
}

void Notecard::DrawCard(float shadowHeight) const
{
	// Shadow
	BeginBlendMode(BLEND_MULTIPLIED);
	DrawRectangleV(Vector2Add(position, { -shadowHeight / 2, shadowHeight }), size, {0,0,0,120});
	EndBlendMode();

	DrawRectangleV(position, size, color);
	int x = position.x + 5;
	int y = position.y + 5;

	// Todo: Replace this with a texture or shader or something.
	{
		int left = position.x;
		int right = position.x + size.x;
		int lineY = y + titleSize;
		DrawLine(left, lineY, right, lineY, cardLineRed);
		int bottom = position.y + size.y;
		BeginBlendMode(BLEND_MULTIPLIED);
		for (lineY += contentSize; lineY < bottom; lineY += contentSize)
		{
			DrawLine(left, lineY, right, lineY, cardLineBlue);
		}
		EndBlendMode();
	}

	DrawText(title.c_str(), x, y, titleSize, BLACK);
	DrawText(content.c_str(), x, y + titleSize + 1, contentSize, BLACK);
}

void Notecard::DrawCardGhost() const
{
	DrawRectangleV(position, size, ColorAlpha(color, 0.85f));
	DrawCircleV(PinPosition(), pinRadius, GOLD);

	int x = position.x + 5;
	int y = position.y + 5;

	DrawText(title.c_str(), x, y, titleSize, BLACK);
	DrawText(content.c_str(), x, y + titleSize + 1, contentSize, BLACK);
}

void Notecard::DrawPin() const
{
	// Shadow
	BeginBlendMode(BLEND_MULTIPLIED);
	DrawCircleV(Vector2Add(PinPosition(), { -1,1 }), pinRadius, {0,0,0,140});
	EndBlendMode();
	DrawCircleV(PinPosition(), pinRadius, DARKGRAY);

	DrawCircleV(PinPosition(), pinRadius-1, GRAY);

	// Shine
	DrawCircleV(Vector2Add(PinPosition(), { pinRadius*0.25,pinRadius*-0.25 }), pinRadius*0.5, LIGHTGRAY);
	DrawCircleV(Vector2Add(PinPosition(), { pinRadius*0.375,pinRadius*-0.375 }), pinRadius*0.25, WHITE);
}

bool Notecard::IsCardHovered(Vector2 point) const
{
	bool hovering =
		(point.x >= position.x) &&
		(point.x <= (position.x + size.x)) &&
		(point.y >= position.y) &&
		(point.y <= (position.y + size.y));

	return hovering;
}

bool Notecard::IsPinHovered(Vector2 point) const
{
	bool hovering =
		Vector2Distance(PinPosition(), point) <= pinRadius;
#if 0
		// This is a likely pointless "forrest for the trees" optimization based upon the assumption that the additional sqrt()
		// of "Vector2Distance()" is more expensive than any additional overhead incurred in this line.
		Vector2LengthSqr(Vector2Subtract(point, position)) <= pinRadius*pinRadius;
#endif
	return hovering;
}

Rectangle Notecard::GetCardRectangle() const
{
	return { position.x, position.y, size.x, size.y };
}

std::vector<Notecard*> g_cards;

void CreateCard(Vector2 position, Color color)
{
	g_cards.push_back(new Notecard(position, color));
}

void DestroyCard(Notecard* card)
{
	auto it = std::find(g_cards.begin(), g_cards.end(), card);
	if (it == g_cards.end()) return;
	g_cards.erase(it);
	for (Thread* thread : card->threads)
	{
		DestroyThread(thread, card);
	}
	delete card;
}

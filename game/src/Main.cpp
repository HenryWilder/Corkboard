#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <algorithm>
#include <variant>
#include <optional>
#include "VectorMath.h"
#include "Thread.h"
#include "Notecard.h"
#include "CustomButtons.h"
#include "Serialize.h"
#include "Elements.h"

#define CORKBOARD CLITERAL(Color){202,164,120,255}

Element draggedElement;
Element hoveredElement;

void CheckHovered(Vector2 cursor)
{
    if (!draggedElement.IsPin()) // Ignore these collision when creating a thread
    {
        // Buttons
        for (ButtonWrapper* button : g_buttons)
        {
            if (button->IsHovered(cursor))
            {
                hoveredElement = button;
                return;
            }
        }

        // Pins
        for (Notecard* card : g_cards)
        {
            if (card->IsPinHovered(cursor))
            {
                hoveredElement = card;
                hoveredElement.SetIsPin();
                return;
            }
        }

        // Threads
        for (Thread* thread : g_threads)
        {
            if (thread->IsHovered(cursor))
            {
                hoveredElement = thread;
                return;
            }
        }
    }

    // Cards
    for (Notecard* card : g_cards)
    {
        if (card->IsCardHovered(cursor))
        {
            hoveredElement = card;
            return;
        }
    }
}

void DrawIcon_CreateNotecard(Vector2 cursor)
{
    float width = 4 * 5;
    float height = 3 * 5;
    float x = cursor.x - width - 2;
    float y = cursor.y - height - 2;
    Rectangle rec = { x, y, width, height };

    Rectangle recShadow = rec;
    recShadow.x -= 2;
    recShadow.y += 2;
    BeginBlendMode(BLEND_MULTIPLIED);
    DrawRectangleLinesEx(recShadow, 2, {0,0,0,70});
    EndBlendMode();

    DrawRectangleLinesEx(rec, 2, cardColor);
}

void DrawIcon_CreateThread(Vector2 cursor)
{
    float width = 4 * 5;
    float x = cursor.x - width - 2;
    float y = cursor.y - width / 2 - 2;
    Vector2 left = { x,y };
    Vector2 right = { x + width,y };

    Vector2 shadowOffset = { -2,2 };
    BeginBlendMode(BLEND_MULTIPLIED);
    DrawLineEx(left + shadowOffset, right + shadowOffset, 2, { 0,0,0,70 });
    EndBlendMode();

    DrawLineEx(left, right, 2, threadColor);
}

void DrawIcon_Move(Vector2 cursor)
{
    // Todo
}

void DrawIcon_Destroy(Vector2 cursor)
{
    float height = 3 * 5;
    float x = cursor.x + 4;
    float y = cursor.y - height - 5;
    float right = x + height;
    float bottom = y + height;

    Vector2 topLeft = { x,y };
    Vector2 topRight = { right,y };
    Vector2 botLeft = { x,bottom };
    Vector2 botRight = { right,bottom };
    Vector2 center = { x + height / 2, y + height / 2 };

    Vector2 shadowOffset = { -2,2 };
    DrawLineEx(topLeft + shadowOffset, botRight + shadowOffset, 2, { 0,0,0,70 });
    DrawLineEx(topRight + shadowOffset, botLeft + shadowOffset, 2, { 0,0,0,70 });
    DrawLineEx(topLeft, botRight, 2, MAROON);
    DrawLineEx(topRight, botLeft, 2, MAROON);
}

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "Corkboard");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    cardColor = cardstockWhite;
    threadColor = threadWhite;

    activeCardColorButton = &g_buttons[0]->genericData;
    activeCardColorButton->state = ButtonState::Active;

    activeThreadColorButton = &g_buttons[9]->genericData;
    activeThreadColorButton->state = ButtonState::Active;

    draggedElement.Clear();

    Vector2 clickOffset = Vector2Zero();

    LoadBoard("testboard.txt");

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        Vector2 cursor = GetMousePosition();

        hoveredElement.Clear(); // Resets each frame

        CheckHovered(cursor);

        // Dragging something
        if (draggedElement.IsSomething())
        {
            // Always lose target on release
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                // Create pin
                // Can't create thread to same card as originator
                if (draggedElement.IsPin() && hoveredElement.IsCardOrPin() && hoveredElement != draggedElement)
                {
                    CreateThread(threadColor, draggedElement.GetCard(), hoveredElement.GetCard());
                }

                // Cleanup
                draggedElement.Clear();
            }

            // Continued dragging
            // Notecard
            else if (draggedElement.IsCard())
            {
                draggedElement.GetCard()->position = cursor + clickOffset;
            }

            // Thread dragging has visual effect but no active effect until release.
        }

        // Handle clicks
        // Clicking has no effect when dragging.
        else if (draggedElement.IsEmpty())
        {
            // Drag a thread from a pin
            if (hoveredElement.IsPin() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                draggedElement = hoveredElement;
            }

            // Double click a notecard
            // Edits card text
            else if (hoveredElement.IsCard() && IsGestureDetected(GESTURE_DOUBLETAP))
            {
                // Todo: edit text
            }

            // Drag a notecard
            else if (hoveredElement.IsCard() && IsGestureDetected(GESTURE_DRAG))
            {
                draggedElement = hoveredElement;

                Notecard* card = draggedElement.GetCard();
                clickOffset = card->position - cursor;

                // Move card to end
                g_cards.erase(std::find(g_cards.begin(), g_cards.end(), card));
                g_cards.push_back(card);
            }

            // Left click the board
            // Creates a notecard
            else if (hoveredElement.IsEmpty() && IsGestureDetected(GESTURE_TAP))
            {
                CreateCard(cursor - Notecard::pinOffset, cardColor);
            }

            // Left click a button
            // Performs the button operation
            else if (hoveredElement.IsButton() && IsGestureDetected(GESTURE_TAP))
            {
                hoveredElement.GetButton()->OnClick();
            }

            // Right click a notecard
            // Destroys it
            if (hoveredElement.IsCard() && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            {
                DestroyCard(hoveredElement.GetCard());
                hoveredElement.Clear();
            }

            // Right click a thread
            // Destroys it
            else if (hoveredElement.IsThread() && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            {
                DestroyThread(hoveredElement.GetThread());
                hoveredElement.Clear();
            }
        }

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(CORKBOARD);

            // Draw cards
            for (Notecard* card : g_cards)
            {
                card->DrawCard();
            }

            // Hovered card/pin
            if (hoveredElement.IsCardOrPin())
            {
                Notecard* card = hoveredElement.GetCard();

                // Hovering pin
                if (hoveredElement.IsPin())
                {
                    DrawRing(card->PinPosition(), Notecard::pinRadius, Notecard::pinRadius + 3, 0, 360, 100, YELLOW);
                }
                // Hovering card
                else
                {
                    DrawRectangleLinesEx(card->GetCardRectangle(), 3, YELLOW);
                    DrawText(TextFormat("%i", card->threads.size()), (int)card->position.x, (int)card->position.y - 10, 8, RED);
                }
            }

            // Dragged pin
            if (draggedElement.IsPin())
            {
                DrawRing(draggedElement.GetCard()->PinPosition(), Notecard::pinRadius, Notecard::pinRadius + 3, 0, 360, 100, BLUE);
            }

            // Draw threads
            for (Thread* thread : g_threads)
            {
                thread->Draw();
            }

            // Hovering thread
            if (hoveredElement.IsThread())
            {
                Thread* thread = hoveredElement.GetThread();

                DrawLineEx(thread->StartPosition(), thread->EndPosition(), thread->thickness, YELLOW);
            }
            
            // Creating thread
            if (draggedElement.IsPin())
            {
                Notecard* startCard = draggedElement.GetCard();
                DrawLineEx(startCard->PinPosition(), cursor, Thread::thickness, threadColor);
            }

            // Draw pins
            for (Notecard* card : g_cards)
            {
                card->DrawPin();
            }

            // Draw ghost of hovered card over everything else
            if (hoveredElement.IsCard())
            {
                Notecard* card = hoveredElement.GetCard();

                card->DrawCardGhost();

                DrawText(TextFormat("%i", card->threads.size()), (int)card->position.x, (int)card->position.y - 10, 8, RED);
                DrawRectangleLinesEx(card->GetCardRectangle(), 3, YELLOW);
            }

            /******************************************
            *   Draw the UI                           *
            ******************************************/
            
            // Not dragging
            if (draggedElement.IsEmpty())
            {
                // Hovering nothing
                if (hoveredElement.IsEmpty())
                {
                    DrawIcon_CreateNotecard(cursor);
                }

                // Hovering a pin
                else if (hoveredElement.IsPin())
                {
                    DrawIcon_CreateThread(cursor);
                }

                // Cards can be moved around
                if (hoveredElement.IsCard())
                {
                    DrawIcon_Move(cursor);
                }

                // Pins and cards can be destroyed with right click
                if (hoveredElement.IsCard() || hoveredElement.IsThread())
                {
                    DrawIcon_Destroy(cursor);
                }
            }

            // Buttons
            for (ButtonWrapper* button : g_buttons)
            {
                button->Draw();
            }

            // Hovered button
            if (hoveredElement.IsButton())
            {
                DrawRectangleLinesEx(hoveredElement.GetButton()->GetRectangle(), 3, YELLOW);
            }

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    SaveBoard("testboard.txt");

    while (!g_cards.empty())
    {
        delete g_cards.back();
        g_cards.pop_back();
    }

    while (!g_threads.empty())
    {
        delete g_threads.back();
        g_threads.pop_back();
    }

    while (!g_buttons.empty())
    {
        delete g_buttons.back();
        g_buttons.pop_back();
    }

    CloseWindow();

	return 0;
}
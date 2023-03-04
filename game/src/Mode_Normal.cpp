#include "raylib.h"
#include "Mode_Normal.h"
#include "VectorMath.h"
#include "Notecard.h"
#include "Thread.h"
#include "CustomButtons.h"
#include "Elements.h"
#include "Modes.h"

Element draggedElement;
Element hoveredElement;
Camera2D cam;

void Mode_Normal_Init()
{
    draggedElement.Clear();
    cam = { 0 };
    cam.zoom = 1;
}

void Mode_Normal_Unload()
{
    // todo
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
    DrawRectangleLinesEx(recShadow, 2, { 0,0,0,70 });
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

double lastClickTime = 0.0f;

void Mode_Normal_Update()
{
    /******************************************
    *   Simulate frame and update variables   *
    ******************************************/

#pragma region Camera control
    Vector2 cursor = GetMousePosition();
    Vector2 cursorInWorld = GetScreenToWorld2D(cursor, cam);
    Vector2 cursorDelta = GetMouseDelta();
    Vector2 cursorDeltaInWorld = cursorDelta / cam.zoom;

    // Pan with middle drag
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
    {
        cam.target -= cursorDeltaInWorld;
    }

    // Zoom with scroll
    if (float scroll = GetMouseWheelMove(); scroll != 0)
    {
        cam.offset = cursor;
        cam.target = cursorInWorld;
        cam.zoom *= scroll * (scroll > 0 ? 1.5f : -0.75f);
        cam.zoom = Clamp(cam.zoom, 0.125f, 8.0f);
    }
#pragma endregion

#pragma region Hover Update
    // Hover checks
    hoveredElement.Clear(); // Resets each frame
    {
        if (!draggedElement.IsPin()) // Ignore these collision when creating a thread
        {
            // Buttons
            for (ButtonWrapper* button : g_buttons)
            {
                if (button->IsHovered(cursor))
                {
                    hoveredElement = button;
                    goto FinishHoverTests;
                }
            }

            // Pins
            for (Notecard* card : g_cards)
            {
                if (card->IsPinHovered(cursorInWorld))
                {
                    hoveredElement = card;
                    hoveredElement.SetIsPin();
                    goto FinishHoverTests;
                }
            }

            // Threads
            for (Thread* thread : g_threads)
            {
                if (thread->IsHovered(cursorInWorld))
                {
                    hoveredElement = thread;
                    goto FinishHoverTests;
                }
            }
        }

        // Cards
        for (Notecard* card : g_cards)
        {
            if (card->IsCardHovered(cursorInWorld))
            {
                hoveredElement = card;
                goto FinishHoverTests;
            }
        }
    }
FinishHoverTests:
#pragma endregion

#pragma region Dragging
    // Dragging something
    if (draggedElement.IsSomething())
    {
        // Always lose target on release
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
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
            draggedElement.GetCard()->position += cursorDeltaInWorld;
        }
    }
#pragma endregion

#pragma region Clicks
    // Handle clicks
    // Clicking has no effect when dragging.
    else if (draggedElement.IsEmpty())
    {
        // Drag a thread from a pin
        if (hoveredElement.IsPin() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            draggedElement = hoveredElement;
        }

        // Drag a notecard
        else if (hoveredElement.IsCard() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            constexpr double doubleClickTimeout = 0.3; // 300 milliseconds
            double time = GetTime();
            // Double click
            if (time - lastClickTime <= doubleClickTimeout)
            {
                SetMode_TextEdit(hoveredElement.GetCard());
                return;
            }
            // Regular click
            else
            {
                draggedElement = hoveredElement;

                Notecard* card = draggedElement.GetCard();

                // Move card to end
                g_cards.erase(std::find(g_cards.begin(), g_cards.end(), card));
                g_cards.push_back(card);
            }
            lastClickTime = time;
        }

        // Left click the board
        // Creates a notecard
        else if (hoveredElement.IsEmpty() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            CreateCard(cursorInWorld - Notecard::pinOffset, cardColor);
        }

        // Left click a button
        // Performs the button operation
        else if (hoveredElement.IsButton() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            hoveredElement.GetButton()->OnClick();
        }

        // Right click a notecard
        // Destroys it
        // Todo: Make a dropdown menu with options like "change color" and "destroy"
        if (hoveredElement.IsCard() && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            DestroyCard(hoveredElement.GetCard());
            hoveredElement.Clear();
        }

        // Right click a thread
        // Destroys it
        else if (hoveredElement.IsThread() && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            DestroyThread(hoveredElement.GetThread());
            hoveredElement.Clear();
        }
    }
#pragma endregion

    /******************************************
    *   Draw the frame                        *
    ******************************************/

    constexpr float highlightThickness = 3.0f;
    constexpr Color highlightColor = YELLOW;

    BeginDrawing(); {

        ClearBackground(corkboardColor);

        /******************************************
        *   Draw the world                        *
        ******************************************/

#pragma region Draw world
        BeginMode2D(cam); {

            // Draw cards
            if (draggedElement.IsCard())
            {
                for (Notecard* card : g_cards)
                {
                    card->DrawCard(card == draggedElement.GetCard() ? 4.0f : 2.0f);
                }
            }
            else
            {
                for (Notecard* card : g_cards)
                {
                    card->DrawCard(2.0f);
                }
            }

            // Hovered pin
            if (draggedElement.IsEmpty() && hoveredElement.IsPin())
            {
                DrawRing(hoveredElement.GetCard()->PinPosition(), Notecard::pinRadius, Notecard::pinRadius + highlightThickness, 0, 360, 100, highlightColor);
            }

            // Dragged pin
            else if (draggedElement.IsPin())
            {
                DrawRing(draggedElement.GetCard()->PinPosition(), Notecard::pinRadius, Notecard::pinRadius + highlightThickness, 0, 360, 100, BLUE);
            }

            // Draw threads
            for (Thread* thread : g_threads)
            {
                thread->Draw();
            }

            // Hovering thread
            if (draggedElement.IsEmpty() && hoveredElement.IsThread())
            {
                Thread* thread = hoveredElement.GetThread();

                DrawLineEx(thread->StartPosition(), thread->EndPosition(), Thread::thickness, highlightColor);
            }

            // Thread being created
            else if (draggedElement.IsPin())
            {
                Notecard* startCard = draggedElement.GetCard();
                DrawLineEx(startCard->PinPosition(), cursorInWorld, Thread::thickness, threadColor);
            }

            // Draw pins
            for (Notecard* card : g_cards)
            {
                card->DrawPin();
            }

            // Draw ghost of hovered card over everything else
            // Does not occur while dragging a card
            if (hoveredElement.IsCard() && !draggedElement.IsCard())
            {
                Notecard* card = hoveredElement.GetCard();

                card->DrawCardGhost();

                DrawText(TextFormat("%i", card->threads.size()), (int)card->position.x, (int)card->position.y - 10, 8, RED);
                DrawRectangleLinesEx(card->GetCardRectangle(), highlightThickness, highlightColor);
            }

        } EndMode2D();
#pragma endregion

        /******************************************
        *   Draw the UI                           *
        ******************************************/

#pragma region Overlay
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
            DrawRectangleLinesEx(hoveredElement.GetButton()->GetRectangle(), highlightThickness, highlightColor);
        }
#pragma endregion

    } EndDrawing();
}

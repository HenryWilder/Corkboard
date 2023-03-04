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
        if (!draggedElement.IsEmpty())
        {
            // Always lose target on release
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                // Create pin
                // Can't create thread to same card as originator
                if (draggedElement.IsPin() && hoveredElement.IsCardOrPin() && hoveredElement != draggedElement)
                {
                    Notecard* startCard = draggedElement.GetCard();
                    Notecard* endCard = hoveredElement.GetCard();

                    // Don't create duplicate connections
                    bool shouldCreateThread = true;
                    {
                        Notecard* cardWithFewerConnections =
                            startCard->threads.size() <= endCard->threads.size()
                            ? startCard
                            : endCard;

                        for (Thread* thread : cardWithFewerConnections->threads)
                        {
                            bool connectionExists =
                                (thread->start == startCard && thread->end == endCard) ||
                                (thread->end == startCard && thread->start == endCard);

                            if (connectionExists)
                            {
                                shouldCreateThread = false;
                                break;
                            }
                        }
                    }

                    if (shouldCreateThread)
                        CreateThread(threadColor, startCard, endCard);
                }

                // Cleanup
                draggedElement.Clear();
            }
            // Notecard
            else if (draggedElement.IsCard())
            {
                draggedElement.GetCard()->position = cursor + clickOffset;
            }
        }

        // Click notecard
        if (hoveredElement.IsCardOrPin() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            draggedElement = hoveredElement;
            if (draggedElement.IsCard())
            {
                Notecard* card = draggedElement.GetCard();
                clickOffset = card->position - cursor;

                // Move card to end
                g_cards.erase(std::find(g_cards.begin(), g_cards.end(), card));
                g_cards.push_back(card);
            }
        }

        // Handle clicks
        // Clicking has no effect when dragging.
        if (!draggedElement.IsEmpty())
        {
            // Left click
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                // Left click the board
                // Creates a notecard
                if (hoveredElement.IsEmpty())
                {
                    CreateCard(cursor - Notecard::pinOffset, cardColor);
                }

                // Left click a button
                // Performs the button operation
                else if (hoveredElement.IsButton())
                {
                    hoveredElement.GetButton()->OnClick();
                }

                // Left clicking a thread does nothing.
            }

            // Right click
            else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            {
                if (!hoveredElement.IsEmpty())
                {
                    // Right click a notecard
                    if (hoveredElement.IsCardOrPin())
                    {
                        DestroyCard(hoveredElement.GetCard());
                        hoveredElement.Clear();
                    }

                    // Right click a thread
                    else if (hoveredElement.IsThread())
                    {
                        DestroyThread(hoveredElement.GetThread());
                        hoveredElement.Clear();
                    }

                    // Right clicking a button does nothing.
                }

                // Right clicking has no effect without an element.
            }
        }

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(CORKBOARD);

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
            
            // Not hovering nor dragging
            if (hoveredElement.IsEmpty() && draggedElement.IsEmpty())
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

            // Hovering a pin
            if (draggedElement.IsEmpty() && hoveredElement.IsPin())
            {
                float width = 4 * 5;
                float height = 2;
                float x = cursor.x - width - 2;
                float y = cursor.y - height - 2 * 5 - 2;
                Rectangle rec = { x, y, width, height };

                Rectangle recShadow = rec;
                recShadow.x -= 2;
                recShadow.y += 2;
                BeginBlendMode(BLEND_MULTIPLIED);
                DrawRectangleLinesEx(recShadow, 2, {0,0,0,70});
                EndBlendMode();

                DrawRectangleLinesEx(rec, 2, threadColor);
            }

            // Buttons
            for (ButtonWrapper* button : g_buttons)
            {
                button->Draw();
            }

            // Hovered button
            if (hoveredElement.IsButton())
            {
                ButtonWrapper* button = hoveredElement.GetButton();

                Rectangle rec = button->GetRectangle();
                DrawRectangleLinesEx(rec, 3, YELLOW);
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
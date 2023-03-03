#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <algorithm>
#include <variant>
#include <optional>
#include "VectorMath.h"
#include "Thread.h"
#include "Notecard.h"
#include "CBButton.h"
#include "Serialize.h"

#define CORKBOARD CLITERAL(Color){202,164,120,255}

using Element = std::variant<Notecard*, Thread*, CBButton*>;
using OptElement = std::optional<Element>;

std::vector<CBButton*> g_buttons = {
    // Card colors
    new CardColorButton(CBButton::size.x * 0, 0, cardstockWhite),
    new CardColorButton(CBButton::size.x * 1, 0, cardstockRed),
    new CardColorButton(CBButton::size.x * 2, 0, cardstockOrange),
    new CardColorButton(CBButton::size.x * 3, 0, cardstockYellow),
    new CardColorButton(CBButton::size.x * 4, 0, cardstockGreen),
    new CardColorButton(CBButton::size.x * 5, 0, cardstockLightBlue),
    new CardColorButton(CBButton::size.x * 6, 0, cardstockBlue),
    new CardColorButton(CBButton::size.x * 7, 0, cardstockLavender),
    new CardColorButton(CBButton::size.x * 8, 0, cardstockPink),

    // Thread colors
    // Todo: Make specialized colors
    new ThreadColorButton(CBButton::size.x * 0, CBButton::size.y, threadWhite),
    new ThreadColorButton(CBButton::size.x * 1, CBButton::size.y, threadRed),
    new ThreadColorButton(CBButton::size.x * 2, CBButton::size.y, threadOrange),
    new ThreadColorButton(CBButton::size.x * 3, CBButton::size.y, threadYellow),
    new ThreadColorButton(CBButton::size.x * 4, CBButton::size.y, threadGreen),
    new ThreadColorButton(CBButton::size.x * 5, CBButton::size.y, threadLightBlue),
    new ThreadColorButton(CBButton::size.x * 6, CBButton::size.y, threadBlue),
    new ThreadColorButton(CBButton::size.x * 7, CBButton::size.y, threadPurple),
    new ThreadColorButton(CBButton::size.x * 8, CBButton::size.y, threadPink),
};

void CheckHovered(Vector2 cursor, OptElement& optHoveredElement, bool& hoveringPin, bool draggingPin)
{
    if (!draggingPin) // Ignore these collision when creating a thread
    {
        // Buttons
        for (CBButton* button : g_buttons)
        {
            if (button->IsHovered(cursor))
            {
                optHoveredElement = button;
                return;
            }
        }

        // Pins
        for (Notecard* card : g_cards)
        {
            if (card->IsPinHovered(cursor))
            {
                optHoveredElement = card;
                hoveringPin = true;
                return;
            }
        }

        // Threads
        for (Thread* thread : g_threads)
        {
            if (thread->IsHovered(cursor))
            {
                optHoveredElement = thread;
                return;
            }
        }
    }

    // Cards
    for (Notecard* card : g_cards)
    {
        if (card->IsCardHovered(cursor))
        {
            optHoveredElement = card;
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

    OptElement optDraggedElement = {};

    bool draggingPin = false; // If optDraggedElement is a card, this says whether the card is being dragged or the pin (making a thread).
    Vector2 clickOffset = Vector2Zero();

    LoadBoard("testboard.txt");

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        Vector2 cursor = GetMousePosition();

        OptElement optHoveredElement = {};
        bool hoveringPin = false; // If optHoveredElement is a card, this says whether the card is being hovered or the pin.

        CheckHovered(cursor, optHoveredElement, hoveringPin, draggingPin);

        // Dragging something
        if (optDraggedElement.has_value())
        {
            Element draggedElement = *optDraggedElement;

            // Always lose target on release
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                // Create pin
                if (draggingPin && // Wants to create thread
                    optHoveredElement.has_value() && // Hovering something
                    std::holds_alternative<Notecard*>(*optHoveredElement) && // Hovering a notecard or pin
                    *optHoveredElement != draggedElement) // Not hovering the same notecard/pin that we originated from
                {
                    _ASSERT_EXPR(std::holds_alternative<Notecard*>(draggedElement), L"Can only drag threads from pins.");

                    Notecard* startCard = std::get<Notecard*>(draggedElement);
                    Notecard* endCard = std::get<Notecard*>(*optHoveredElement);

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
                optDraggedElement = {};
                draggingPin = false;
            }
            else
            {
                // Notecard
                if (std::holds_alternative<Notecard*>(draggedElement))
                {
                    if (!draggingPin)
                    {
                        std::get<Notecard*>(draggedElement)->position = cursor + clickOffset;
                    }
                }
            }
        }

        // Hovering something
        if (optHoveredElement.has_value())
        {
            Element hoveredElement = *optHoveredElement;

            // Notecard
            if (std::holds_alternative<Notecard*>(hoveredElement))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    optDraggedElement = hoveredElement;
                    if (hoveringPin)
                    {
                        draggingPin = true;
                    }
                    else
                    {
                        Notecard* card = std::get<Notecard*>(hoveredElement);
                        clickOffset = card->position - cursor;
                        auto it = std::find(g_cards.begin(), g_cards.end(), card);
                        g_cards.erase(it);
                        g_cards.push_front(card);
                    }
                }
            }
        }

        // Handle clicks
        // Clicking has no effect when dragging.
        if (!optDraggedElement.has_value())
        {
            // Left click
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                // Left click the board
                // Creates a notecard
                if (!optHoveredElement.has_value())
                {
                    CreateCard(cursor - Notecard::pinOffset, cardColor);
                }

                // Left click a button
                // Performs the button operation
                else if (std::holds_alternative<CBButton*>(*optHoveredElement))
                {
                    std::get<CBButton*>(*optHoveredElement)->OnClick();
                }

                // Left clicking a thread does nothing.
            }

            // Right click
            else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            {
                if (optHoveredElement.has_value())
                {
                    Element hoveredElement = *optHoveredElement;

                    // Right click a notecard
                    if (std::holds_alternative<Notecard*>(hoveredElement))
                    {
                        DestroyCard(std::get<Notecard*>(hoveredElement));
                        optHoveredElement = {};
                    }

                    // Right click a thread
                    else if (std::holds_alternative<Thread*>(hoveredElement))
                    {
                        DestroyThread(std::get<Thread*>(hoveredElement));
                        optHoveredElement = {};
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

            // Cards are drawn in reverse order
            for (int i = g_cards.size() - 1; i >= 0; --i)
            {
                g_cards[i]->DrawCard();
            }

            // Hovered card/pin
            if (optHoveredElement.has_value() && std::holds_alternative<Notecard*>(*optHoveredElement))
            {
                Notecard* card = std::get<Notecard*>(*optHoveredElement);

                // Hovering pin
                if (hoveringPin)
                {
                    DrawRing(card->PinPosition(), Notecard::pinRadius, Notecard::pinRadius + 3, 0, 360, 100, YELLOW);
                }
                // Hovering card
                else
                {
                    DrawRectangleLinesEx(card->GetCardRectangle(), 3, YELLOW);
                    DrawText(TextFormat("%i", card->threads.size()), card->position.x, card->position.y - 10, 8, RED);
                }
            }

            // Dragged card/pin
            if (optDraggedElement.has_value() && std::holds_alternative<Notecard*>(*optDraggedElement))
            {
                Notecard* card = std::get<Notecard*>(*optDraggedElement);

                // Hovering pin
                if (draggingPin)
                {
                    DrawRing(card->PinPosition(), Notecard::pinRadius, Notecard::pinRadius + 3, 0, 360, 100, BLUE);
                }
            }

            for (Thread* thread : g_threads)
            {
                thread->Draw();
            }

            // Hovering thread
            if (optHoveredElement.has_value() && std::holds_alternative<Thread*>(*optHoveredElement))
            {
                Thread* thread = std::get<Thread*>(*optHoveredElement);

                DrawLineEx(thread->StartPosition(), thread->EndPosition(), thread->thickness, YELLOW);
            }
            
            // Creating thread
            if (draggingPin)
            {
                // draggingPin implies that we have already hovered and clicked a pin on a notecard and have not yet released.
                _ASSERT_EXPR(optDraggedElement.has_value() && std::holds_alternative<Notecard*>(*optDraggedElement), L"Dragging a pin without having a notecard selected");

                Notecard* startCard = std::get<Notecard*>(*optDraggedElement);
                DrawLineEx(startCard->PinPosition(), cursor, Thread::thickness, threadColor);
            }

            for (Notecard* card : g_cards)
            {
                card->DrawPin();
            }

            // Draw ghost of hovered card over everything else
            if (optHoveredElement.has_value() && std::holds_alternative<Notecard*>(*optHoveredElement) && !hoveringPin)
            {
                Notecard* card = std::get<Notecard*>(*optHoveredElement);

                card->DrawCardGhost();

                DrawText(TextFormat("%i", card->threads.size()), card->position.x, card->position.y - 10, 8, RED);
                DrawRectangleLinesEx(card->GetCardRectangle(), 3, YELLOW);
            }

            /******************************************
            *   Draw the UI                           *
            ******************************************/
            
            // Not hovering nor dragging
            if (!optHoveredElement.has_value() && !optDraggedElement.has_value())
            {
                int width = 4 * 5;
                int height = 3 * 5;
                Rectangle rec = { cursor.x - width - 2, cursor.y - height - 2, width, height };

                Rectangle recShadow = rec;
                recShadow.x -= 2;
                recShadow.y += 2;
                BeginBlendMode(BLEND_MULTIPLIED);
                DrawRectangleLinesEx(recShadow, 2, {0,0,0,70});
                EndBlendMode();

                DrawRectangleLinesEx(rec, 2, cardColor);
            }

            // Hovering a pin
            if (!optDraggedElement.has_value() && hoveringPin)
            {
                _ASSERT_EXPR(optHoveredElement.has_value() && std::holds_alternative<Notecard*>(*optHoveredElement), L"Hovering a pin without actually hovering a pin");

                int width = 4 * 5;
                int height = 2;
                Rectangle rec = { cursor.x - width - 2, cursor.y - height - 2 * 5 - 2, width, height };

                Rectangle recShadow = rec;
                recShadow.x -= 2;
                recShadow.y += 2;
                BeginBlendMode(BLEND_MULTIPLIED);
                DrawRectangleLinesEx(recShadow, 2, {0,0,0,70});
                EndBlendMode();

                DrawRectangleLinesEx(rec, 2, threadColor);
            }

            // Buttons
            for (CBButton* button : g_buttons)
            {
                button->Draw();
            }

            // Hovered button
            if (optHoveredElement.has_value() && std::holds_alternative<CBButton*>(*optHoveredElement))
            {
                CBButton* button = std::get<CBButton*>(*optHoveredElement);

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
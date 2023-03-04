#include "Modes.h"
#include "Mode_TextEdit.h"
#include "Notecard.h"
#include "Thread.h"
#include "CustomButtons.h"

Notecard* cardBeingEdited;

void Mode_TextEdit_Init()
{
	cardBeingEdited = nullptr;
}

void Mode_TextEdit_Unload()
{
	// todo
}

void Mode_TextEdit_Update()
{
    bool hoveringEditedCard = cardBeingEdited->IsCardHovered(GetScreenToWorld2D(GetMousePosition(), GetNormalModeCamera()));

    if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !hoveringEditedCard) || IsKeyPressed(KEY_ESCAPE))
    {
        SetMode_Normal();
        PollInputEvents(); // Consume click
        return;
    }

    BeginDrawing(); {

        ClearBackground(corkboardColor);

        /******************************************
        *   Draw the world                        *
        ******************************************/

#pragma region Draw world
        BeginMode2D(GetNormalModeCamera()); {

            for (Notecard* card : g_cards)
            {
                if (card == cardBeingEdited)
                    continue;
                card->DrawCard(2.0f);
            }

            // Draw threads
            for (Thread* thread : g_threads)
            {
                thread->Draw();
            }

            // Draw pins
            for (Notecard* card : g_cards)
            {
                card->DrawPin();
            }

            cardBeingEdited->DrawCard(6.0f);

        } EndMode2D();
#pragma endregion

        /******************************************
        *   Draw the UI                           *
        ******************************************/

#pragma region Overlay

        // Buttons
        for (ButtonWrapper* button : g_buttons)
        {
            button->Draw();
        }

#pragma endregion

    } EndDrawing();
}

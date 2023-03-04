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
#include "Modes.h"
#include "Mode_Normal.h"
#include "Mode_TextEdit.h"

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

    Mode_Normal_Init();
    Mode_TextEdit_Init();

    SetMode_Normal();

    LoadBoard("testboard.txt"); 

    while (!WindowShouldClose())
    {
        bool modeChange = false;
        do
        {
            switch (GetMode())
            {
            case Mode::Normal:
                modeChange = Mode_Normal_Update();
                break;
            case Mode::TextEdit:
                modeChange = Mode_TextEdit_Update();
                break;
            }
        } while (modeChange);
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    Mode_TextEdit_Unload();
    Mode_Normal_Unload();

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
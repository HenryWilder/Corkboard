#pragma once

enum class Mode
{
    Normal,
    TextEdit,
};

class Notecard;

void Mode_Normal_Init();
bool Mode_Normal_Update();
void Mode_Normal_Unload();

void Mode_TextEdit_Init();
bool Mode_TextEdit_Update();
void Mode_TextEdit_Unload();

void SetMode_Normal();
void SetMode_TextEdit(Notecard* editing);
Mode GetMode();

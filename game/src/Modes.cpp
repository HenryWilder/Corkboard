#include "Modes.h"
#include "Mode_Normal.h"
#include "Mode_TextEdit.h"

Mode mode;

extern Notecard* cardBeingEdited;
extern Camera2D cam;

void SetMode_Normal()
{
	mode = Mode::Normal;
	cardBeingEdited = nullptr;
}

void SetMode_TextEdit(Notecard* editing)
{
	mode = Mode::TextEdit;
	cardBeingEdited = editing;
}

Mode GetMode()
{
	return mode;
}

Camera2D GetNormalModeCamera()
{
	return cam;
}

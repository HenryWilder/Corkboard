#include "Mode_TextEdit.h"
#include "Notecard.h"

Notecard* cardBeingEdited;

void Mode_TextEdit_Init()
{
	cardBeingEdited = nullptr;
}

void Mode_TextEdit_Unload()
{
	// todo
}

bool Mode_TextEdit_Update()
{
	return false;
}

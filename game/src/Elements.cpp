#include "Elements.h"

bool Element::IsEmpty() { return tag == Type::ELEMENTTYPE_EMPTY; }
bool Element::IsCard() { return tag == Type::ELEMENTTYPE_CARD; }
bool Element::IsPin() { return tag == Type::ELEMENTTYPE_PIN; }
bool Element::IsCardOrPin() { return IsCard() || IsPin(); }
bool Element::IsThread() { return tag == Type::ELEMENTTYPE_THREAD; }
bool Element::IsButton() { return tag == Type::ELEMENTTYPE_BUTTON; }

void Element::Clear() { tag = Type::ELEMENTTYPE_EMPTY; }
void Element::SetIsPin() { _ASSERT(IsCard()); tag = Type::ELEMENTTYPE_PIN; }

Notecard* Element::GetCard() { _ASSERT(IsCardOrPin()); return card; }
void Element::operator=(Notecard* other) { card = other; tag = Type::ELEMENTTYPE_CARD; }

Thread* Element::GetThread() { _ASSERT(IsThread()); return thread; }
void Element::operator=(Thread* other) { thread = other; tag = Type::ELEMENTTYPE_THREAD; }

ButtonWrapper* Element::GetButton() { _ASSERT(IsButton()); return button; }
void Element::operator=(ButtonWrapper* other) { button = other; tag = Type::ELEMENTTYPE_BUTTON; }

void Element::operator=(Element& other)
{
	tag = other.tag;

	switch (tag)
	{
	case Type::ELEMENTTYPE_EMPTY: break;
	case Type::ELEMENTTYPE_PIN:
	case Type::ELEMENTTYPE_CARD: card = other.card; break;
	case Type::ELEMENTTYPE_THREAD: thread = other.thread; break;
	case Type::ELEMENTTYPE_BUTTON: button = other.button; break;
	}
}

bool Element::operator==(Element& other)
{
	if (tag != other.tag)
		return false;

	switch (tag)
	{
	case Type::ELEMENTTYPE_EMPTY: return true;
	case Type::ELEMENTTYPE_CARD:
	case Type::ELEMENTTYPE_PIN: return card == other.card;
	case Type::ELEMENTTYPE_THREAD: return thread == other.thread;
	case Type::ELEMENTTYPE_BUTTON: return button == other.button;
	}

	return false;
}

bool Element::operator!=(Element& other)
{
	if (tag != other.tag)
		return true;

	switch (tag)
	{
	case Type::ELEMENTTYPE_EMPTY: return false;
	case Type::ELEMENTTYPE_CARD:
	case Type::ELEMENTTYPE_PIN: return card != other.card;
	case Type::ELEMENTTYPE_THREAD: return thread != other.thread;
	case Type::ELEMENTTYPE_BUTTON: return button != other.button;
	}

	return true;
}

#pragma once
#include <crtdbg.h>

class Notecard;
class Thread;
class ButtonWrapper;

struct Element
{
private:
    enum class Type
    {
        ELEMENTTYPE_EMPTY = 0,
        ELEMENTTYPE_CARD,
        ELEMENTTYPE_PIN,
        ELEMENTTYPE_THREAD,
        ELEMENTTYPE_BUTTON,
    } tag;

    union
    {
        Notecard* card;
        Thread* thread;
        ButtonWrapper* button;
    };

public:
    Element() : tag(Type::ELEMENTTYPE_EMPTY), card(nullptr) {}

    bool IsEmpty();
    bool IsCard();
    bool IsPin();
    bool IsCardOrPin(); // Both pin and card share Notecard*
    bool IsThread();
    bool IsButton();

    void Clear();
    void SetIsPin();

    Notecard* GetCard();
    void operator=(Notecard* other);

    Thread* GetThread();
    void operator=(Thread* other);

    ButtonWrapper* GetButton();
    void operator=(ButtonWrapper* other);

    void operator=(Element& other);
    bool operator==(Element& other);
    bool operator!=(Element& other);
};

#include "Thread.h"
#include "Notecard.h"
#include "VectorMath.h"

Vector2 Thread::StartPosition() const
{
    return start->PinPosition();
}

Vector2 Thread::EndPosition() const
{
    return end->PinPosition();
}

void Thread::Draw() const
{
    Vector2 shadowOffset = {-2,2};

    Vector2 startPos = StartPosition();
    Vector2 endPos = EndPosition();

    // Shadow
    BeginBlendMode(BLEND_MULTIPLIED);
    DrawLineEx(startPos + shadowOffset, endPos + shadowOffset, thickness, {0,0,0,100});
    EndBlendMode();

    DrawLineEx(startPos, endPos, thickness, color);
}

bool Thread::IsHovered(Vector2 point) const
{
    Vector2 startPos = StartPosition();
    Vector2 endPos = EndPosition();

    // Start and end overlap
    if (startPos == endPos)
    {
        bool pointMatches = startPos == point; // Point overlaps singularity

        return pointMatches;
    }

    float lengthSquared = Vector2DistanceSqr(endPos, startPos);

    Vector2 endRelativeToStart = endPos - startPos;
    float t = Clamp(Vector2DotProduct(point - startPos, endRelativeToStart) / lengthSquared, 0, 1);
    Vector2 projection = startPos + endRelativeToStart * t;

    bool hovering = Vector2DistanceSqr(point, projection) <= thickness * thickness;

    return hovering;
}

std::vector<Thread*> g_threads;

void CreateThread(Color color, Notecard* start, Notecard* end)
{
    Thread* thread = new Thread(color, start, end);
	g_threads.push_back(thread);
    start->AddThreadConnection(thread);
    end->AddThreadConnection(thread);
}

void DestroyThread(Thread* thread, Notecard* ignoring)
{
    auto it = std::find(g_threads.begin(), g_threads.end(), thread);
    if (it != g_threads.end())
    {
        g_threads.erase(it);
        if (thread->start != ignoring)
            thread->start->RemoveThreadConnection(thread);
        if (thread->end != ignoring)
            thread->end->RemoveThreadConnection(thread);
        delete thread;
    }
    else
    {
        _ASSERT_EXPR(false, L"Thread was created by unorthodox means, or has already been destroyed.");
    }
}

#pragma once
#include <vector>
#include <algorithm>

// i think i understand templates now :D
template<typename TEvent, typename TValue, typename Accessor>
TValue FindLastEventBeforeOrAt(
    float newTime,
    const std::vector<TEvent>& events,
    TValue defaultValue,
    Accessor valueAccessor)
{
    if (events.empty())
        return defaultValue;

    auto it = std::upper_bound(
        events.begin(),
        events.end(),
        newTime,
        [](float t, const TEvent& e) { return t < e.Time; }
    );

    if (it == events.begin())
        return defaultValue;

    --it;
    return valueAccessor(*it);
}

template<typename TEvent>
int FindNextEventIndex(float newTime, const std::vector<TEvent>& events)
{
    auto it = std::upper_bound(
        events.begin(),
        events.end(),
        newTime,
        [](float t, const TEvent& e) { return t < e.Time; }
    );

    return static_cast<int>(it - events.begin());
}

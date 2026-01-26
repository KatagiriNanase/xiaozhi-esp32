#include "context.hpp"

Event& Context::getEvent()
{
    return event_;
}

Manager& Context::getManager()
{
    return manager_;
}

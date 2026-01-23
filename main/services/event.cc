#include "event.hpp"

boost::signals2::connection Event::connectEventSignal(void* obj, EventSignal::slot_type slot_fun)
{
    return event_map_[obj].connect(slot_fun);
}

void Event::sendEvent(void* obj, lv_event_t* e)
{
    event_map_[obj](e);
}

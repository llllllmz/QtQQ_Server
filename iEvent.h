#pragma once
#include "def_eventType.h"

// iEvent接口
class iEvent {
public:
    iEvent(EventType type) : m_type(type) {}
    virtual ~iEvent() {}
    EventType getEventType()const{return m_type;}
    
private:
    EventType m_type;
};

#pragma once
#include "iEvent.h"
#include <string>
#include "muduo/base/Types.h"
using namespace muduo;
// 处理器接口
class iEventHandler {
public:
    iEventHandler(const std::string&name):m_name(name){};
    virtual ~iEventHandler() {}
    std::string& get_name() { return m_name; };

    virtual iEvent* handle(const iEvent* event) = 0;
    // virtual iEvent* parseEvent(EventType eventTpye, const muduo::string& message);

private:
    std::string m_name;
};
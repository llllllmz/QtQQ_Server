#include "EventDispatcher.h"
#include "muduo/base/Logging.h"
#include <algorithm>
#include "def_eventType.h"
#include "def_events.h"
#include "nlohmann/json.hpp"
#include "QQServer.h"
#include "./entity/Message.hpp"
using json = nlohmann::json;

EventDispatcher* EventDispatcher::instance = nullptr;
EventDispatcher* EventDispatcher::getInstance()
{
    if(instance == nullptr)
        instance = new EventDispatcher;
    return instance;
}

bool EventDispatcher::open()
{
    m_svr_exit = false;
    return true;
}
void EventDispatcher::close()
{
    m_svr_exit = true;
    m_handlers.clear();
}

void EventDispatcher::subscribe(EventType et, iEventHandler* handler)
{
    T_EventHandlersMap::iterator iter = m_handlers.find(et);
    if(iter != m_handlers.end())
    {
        T_EventHandlers::iterator handler_iter = std::find(iter->second.begin(),iter->second.end(),handler);
        if(handler_iter == iter->second.end())
        {
            iter->second.push_back(handler);
        }
    }
    else
    {
        m_handlers[et].push_back(handler);
    }
}
void EventDispatcher::unsubscribe(EventType et, iEventHandler* handler)
{
    T_EventHandlersMap::iterator iter = m_handlers.find(et);
    if(iter != m_handlers.end())
    {
        T_EventHandlers::iterator handler_iter = std::find(iter->second.begin(),iter->second.end(),handler);
        if(handler_iter != iter->second.end())
        {
            iter->second.erase(handler_iter);
        }
    }
}

iEvent* EventDispatcher::parseEvent(const muduo::net::TcpConnectionPtr& conn,EventType eventTpye, const muduo::string& message)
{
    LOG_DEBUG << "EventDispatcher::svr: recv message: " << message;
       
    if(eventTpye == ET_LoginRequest)
    {
        //todo json库解析
        json data = json::parse(message);
        std::string username = data["username"];
        std::string password = data["password"];
        LOG_DEBUG << "EventDispatcher::svr: recv username: " << username << " password: " << password;
        LoginRequest* event = new LoginRequest();
        bool success = event->from_json(data);
        if(!success)
        {
            delete event;
            LOG_ERROR << "EventDispatcher::svr: parse json failed!";
            return nullptr;
        }
        return event;
    }
    else if(eventTpye == ET_UserGroupRequest)
    {
        LOG_INFO << "EventDispatcher::svr: recv UserGroupRequest";
        //从conn 中获取user
        User user = boost::any_cast<User>(conn->getContext());
        UserGroupRequest* event = new UserGroupRequest(user.get_id());
        return event;   
    }
    else if(eventTpye == ET_UserListRequest)
    {
        LOG_INFO << "EventDispatcher::svr: recv UserListRequest";
        //从conn 中获取user
        User user = boost::any_cast<User>(conn->getContext());
        UserListRequest* event = new UserListRequest(user.get_id());
        return event;   
    }
    else if(eventTpye == ET_GroupListRequest)
    {
        LOG_INFO << "EventDispatcher::svr: recv GroupListRequest";
        User user = boost::any_cast<User>(conn->getContext());
        GroupListRequest* event = new GroupListRequest(user.get_id());
        return event;
    }
    else if(eventTpye == ET_GroupMembersRequest)
    {
        LOG_INFO << "EventDispatcher::svr: recv GroupMembersRequest";
        json data = json::parse(message);
        int groupid = data["groupid"];
        GroupMembersRequest* event = new GroupMembersRequest(groupid);
        return event;

    }
    else if(eventTpye == ET_MessageSendRequest)
    {
        LOG_INFO << "EventDispatcher::svr: recv MessageSendRequest";
        json data = json::parse(message);
        //
        User user = boost::any_cast<User>(conn->getContext());
        Message message;
        message.from_json(data);
        message.setSenderId(user.get_id());
        MessageSendRequest* event = new MessageSendRequest(message);
        
        return event;
    }
    else if(eventTpye == ET_MessageRead)
    {
        LOG_INFO << " EventDispatcher::svr: recv MessageRead";
        json data = json::parse(message);
        //
        User user = boost::any_cast<User>(conn->getContext());
        Message message;
        message.from_json(data);
        message.setReceiverId(user.get_id());
        MessageReadRequest* event = new MessageReadRequest(message);
        
        return event;
    }
    else if(eventTpye == ET_MessageListRequest)
    {
        LOG_INFO << "EventDispatcher::svr: recv MessageListRequest";
        json data = json::parse(message);
        //
        // User user = boost::any_cast<User>(conn->getContext());//todo modify后期要从context中判断消息种类 从conn 中获取user
        Message message;
        message.from_json(data);
        // message.setReceiverId(user.get_id());
       MessageListRequest* event = new MessageListRequest(message);
       return event;
    }
    return nullptr;
}

void EventDispatcher::svr(const muduo::net::TcpConnectionPtr& conn,EventType eventType, const muduo::string message)
{
    LOG_DEBUG << "EventDispatcher::svr: recv event: " << eventType;
    if(!m_svr_exit)
    {
        iEvent* req_ev = parseEvent(conn,eventType,message);
        iEvent* res_ev = dispatch(req_ev);
        if(res_ev == nullptr)
        {
            LOG_ERROR << "EventDispatcher::svr: dispatch failed!";
            
        }
        responseEvent(conn,res_ev);
    }
    else
    {
        LOG_WARN << "EventDispatcher::svr: server not open! ";
    }
}

iEvent* EventDispatcher::dispatch(const iEvent* ev)
{
    
    EventType eventTpye = ev->getEventType();
    T_EventHandlersMap::iterator iter = m_handlers.find(eventTpye);
    if(iter == m_handlers.end())
    {
        LOG_WARN << "EventDispatcher : no any event handler subscribed " << eventTpye;
        return nullptr;
    }

    iEvent* rsp = nullptr;
    for(auto hdl_iter=iter->second.begin();hdl_iter!=iter->second.end();hdl_iter++)
    {
        iEventHandler* handler = *hdl_iter;
        LOG_DEBUG << "EventDispatcher : get handler: "<<handler->get_name();

        rsp = handler->handle(ev);
    }
    
    return rsp;

}

void EventDispatcher::responseEvent(const muduo::net::TcpConnectionPtr& conn,const iEvent* ev)
{
    if(ev == nullptr)
    {
        LOG_ERROR << "EventDispatcher::responseEvent: response event is nullptr!";
        return ;
    }
    if(ev->getEventType() == ET_LoginResponse)
    {
        LOG_DEBUG << "EventDispatcher::responseEvent: send login response";
        LoginResponse* res_ev = (LoginResponse*)ev;
        if(res_ev->get_result())
        {
            //把user保存到conn
            conn->setContext(res_ev->get_user());
            //
            //加锁
            std::lock_guard<std::mutex> lock(g_mutex);
            g_userIdToConn[res_ev->get_user().get_id()] = conn;//todo 将已在线的conn移除并通知客户端
            LOG_INFO << "EventDispatcher::responseEvent: login success!";
        }
        
        std::string body = res_ev->to_json().dump();
        //发送类型EventType
        struct header buffer;
        short eventType = ET_LoginResponse;
        eventType = htons(eventType);
        buffer.type = eventType;
        int len = body.size();
        char* buf = new char[10 + len];
        len = htonl(len);
        
        buffer.len = len;
        memcpy(buffer.magic, "abcd", 4);

        memcpy(buf, &buffer.magic, 4);
        memcpy(buf+4,&buffer.type, 2);
        memcpy(buf+6,&buffer.len, 4);
        //打印buffer的sizeof
        LOG_DEBUG << sizeof(buffer);
        memcpy(buf+10, body.c_str(),body.size());
        muduo::string message(buf,10+body.size());
        conn->send(message);

    } 
    else if(ev->getEventType() == ET_UserGroupResponse)
    {
        LOG_DEBUG << "EventDispatcher::responseEvent: send user group response";
        UserGroupResponse* res_ev = (UserGroupResponse*)ev;
        std::string body = res_ev->to_json().dump();
        //发送类型EventType
        struct header buffer;
        short eventType = ET_UserGroupResponse;
        eventType= htons(eventType);
        buffer.type = eventType;
        int len = body.size();
        char* buf = new char[10 + len];
        len = htonl(len);
        buffer.len = len;
        memcpy(buffer.magic, "abcd", 4);
        
        memcpy(buf, &buffer.magic, 4);
        memcpy(buf+4,&buffer.type, 2);
        memcpy(buf+6,&buffer.len, 4);
        memcpy(buf+10, body.c_str(),body.size());
        muduo::string message(buf,10+body.size());
        
        conn->send(message);
        // delete res_ev;
        return;
    }
    else if(ev->getEventType() == ET_UserListResponse)
    {
        LOG_DEBUG << "EventDispatcher::responseEvent: send user list response";
        UserListResponse* res_ev = (UserListResponse*)ev;
        std::string body = res_ev->to_json().dump();
        //发送类型EventType
        struct header buffer;
        short eventType = ET_UserListResponse;
        eventType= htons(eventType);
        buffer.type = eventType;
        int len = body.size();
        char* buf = new char[10 + len];
        len = htonl(len);
        buffer.len = len;
        memcpy(buffer.magic, "abcd", 4);
        
        memcpy(buf, &buffer.magic, 4);
        memcpy(buf+4,&buffer.type, 2);
        memcpy(buf+6,&buffer.len, 4);
        memcpy(buf+10, body.c_str(),body.size());
        muduo::string message(buf,10+body.size());
        conn->send(message);
        delete res_ev;
        return;
    }
    else if(ev->getEventType() == ET_GroupListResponse)
    {
        LOG_DEBUG << "EventDispatcher::responseEvent: send group list response";
        GroupListResponse* res_ev = (GroupListResponse*)ev;
        std::string body = res_ev->to_json().dump();
        //发送类型EventType
        struct header buffer;
        short eventType = ET_GroupListResponse;
        eventType= htons(eventType);
        
        buffer.type = eventType;
        int len = body.size();
        char* buf = new char[10 + len];
        len = htonl(len);
        buffer.len = len;
        memcpy(buffer.magic, "abcd", 4);
        
        memcpy(buf, &buffer.magic, 4);
        memcpy(buf+4,&buffer.type, 2);
        memcpy(buf+6,&buffer.len, 4);
        memcpy(buf+10, body.c_str(),body.size());
        muduo::string message(buf,10+body.size());
        conn->send(message);
        delete res_ev;
        return;
 
    }
    else if(ev->getEventType() == ET_GroupMembersResponse)
    {
        LOG_DEBUG << "EventDispatcher::responseEvent: send group members response";
        GroupMembersResponse* res_ev = (GroupMembersResponse*)ev;
        std::string body = res_ev->to_json().dump();
        //发送类型EventType
        struct header buffer;
        short eventType = ET_GroupMembersResponse;
        eventType= htons(eventType);
        buffer.type = eventType;
        int len = body.size();
        char* buf = new char[10 + len];
        len = htonl(len);
        buffer.len = len;
        memcpy(buffer.magic, "abcd", 4);
        
        memcpy(buf, &buffer.magic, 4);
        memcpy(buf+4,&buffer.type, 2);
        memcpy(buf+6,&buffer.len, 4);
        memcpy(buf+10, body.c_str(),body.size());
        muduo::string message(buf,10+body.size());
        conn->send(message);
        delete res_ev;
        return;
    }
    else if(ev->getEventType() == ET_MessageSendResponse)
    {
        LOG_DEBUG << "EventDispatcher::responseEvent: send message send response";
        MessageSendResponse* res_ev = (MessageSendResponse*)ev;
        std::string body = res_ev->to_json().dump();
        //发送类型EventType
        struct header buffer;
        short eventType = ET_MessageSendResponse;
        eventType= htons(eventType);
        buffer.type = eventType;
        int len = body.size();
        char* buf = new char[10 + len];
        len = htonl(len);
        buffer.len = len;
        memcpy(buffer.magic, "abcd", 4);
        
        memcpy(buf, &buffer.magic, 4);
        memcpy(buf+4,&buffer.type, 2);
        memcpy(buf+6,&buffer.len, 4);
        memcpy(buf+10, body.c_str(),body.size());
        muduo::string message(buf,10+body.size());
        conn->send(message);
        delete res_ev;
        return;
    }
    else if(ev->getEventType() == ET_MessageListResponse)
    {
        
        LOG_DEBUG << "EventDispatcher::responseEvent: send message list response";
        MessageListResponse* res_ev = (MessageListResponse*)ev;
        std::string body = res_ev->to_json().dump();
        struct header buffer;
        short eventType = ET_MessageListResponse;
        eventType= htons(eventType);
        buffer.type = eventType;
        int len = body.size();
        char* buf = new char[10 + len];
        len = htonl(len);
        buffer.len = len;
        memcpy(buffer.magic, "abcd", 4);
        
        memcpy(buf, &buffer.magic, 4);
        memcpy(buf+4,&buffer.type, 2);
        memcpy(buf+6,&buffer.len, 4);
        memcpy(buf+10, body.c_str(),body.size());
        muduo::string message(buf,10+body.size());
        conn->send(message);
        delete res_ev;
        return;

    }
    
    else
    {
        LOG_WARN << "EventDispatcher::responseEvent: unknow event type: " << ev->getEventType();
    }
}
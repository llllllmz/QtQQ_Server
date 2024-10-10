#include "def_eventHandler.h"
#include "def_events.h"
#include "muduo/base/Logging.h"
#include "EventDispatcher.h"
#include "dao/UserDao.hpp"
#include "dao/UserGroupDao.hpp"
#include "dao/MessageDao.hpp"
#include "QQServer.h"
#include <iostream>
UserEventHandler::UserEventHandler()
    :iEventHandler("UserEventHandler")
{
    //订阅
    EventDispatcher::getInstance()->subscribe(ET_LoginRequest,this);
    EventDispatcher::getInstance()->subscribe(ET_UserGroupRequest,this);
    EventDispatcher::getInstance()->subscribe(ET_UserListRequest,this);
    EventDispatcher::getInstance()->subscribe(ET_GroupListRequest,this);
    EventDispatcher::getInstance()->subscribe(ET_GroupMembersRequest,this);
    EventDispatcher::getInstance()->subscribe(ET_MessageSendRequest,this);
    // EventDispatcher::getInstance()->subscribe(ET_MessageReceiveAck,this);
    EventDispatcher::getInstance()->subscribe(ET_MessageRead,this);
    EventDispatcher::getInstance()->subscribe(ET_MessageListRequest,this);
}
UserEventHandler::~UserEventHandler()
{
    //取消订阅
    EventDispatcher::getInstance()->unsubscribe(ET_LoginRequest,this);
    EventDispatcher::getInstance()->unsubscribe(ET_UserGroupRequest,this);
    EventDispatcher::getInstance()->unsubscribe(ET_UserListRequest,this);
    EventDispatcher::getInstance()->unsubscribe(ET_GroupListRequest,this);
    EventDispatcher::getInstance()->unsubscribe(ET_GroupMembersResponse,this);
    EventDispatcher::getInstance()->unsubscribe(ET_MessageSendRequest,this);
    // EventDispatcher::getInstance()->unsubscribe(ET_MessageReceiveAck,this);
    EventDispatcher::getInstance()->unsubscribe(ET_MessageRead,this);
        EventDispatcher::getInstance()->unsubscribe(ET_MessageListRequest,this);
}
iEvent* UserEventHandler::handle(const iEvent* event)
{
    if(event->getEventType() == ET_LoginRequest)
    {
        return handleLoginRequest(dynamic_cast<const LoginRequest*>(event));
    }
    else if(event->getEventType() == ET_UserGroupRequest)
    {
        return handleUserGroupRequest(dynamic_cast<const UserGroupRequest*>(event));
    } 
    else if(event->getEventType() == ET_UserListRequest)
    {
        return handleUserListRequest(dynamic_cast<const UserListRequest*>(event));
    }
    else if(event->getEventType() == ET_GroupListRequest)
    {
        return handleGroupListRequest(dynamic_cast<const GroupListRequest*>(event));
    }
    else if(event->getEventType() == ET_GroupMembersRequest)
    {
        return handleGroupMembersRequest(dynamic_cast<const GroupMembersRequest*>(event));
    }
    else if(event->getEventType() == ET_MessageSendRequest)
    {
        return handleMessageSendRequest(dynamic_cast<const MessageSendRequest*>(event));
    }
    else if(event->getEventType() == ET_MessageRead)
    {
        return handleMessageRead(dynamic_cast<const MessageReadRequest*>(event));
    }
    else if(event->getEventType() == ET_MessageListRequest)
    {
        return handleMessageListRequest(dynamic_cast<const MessageListRequest*>(event));
    }
    

}

LoginResponse* UserEventHandler::handleLoginRequest(const LoginRequest* event)
{
        //日志
    LOG_INFO << "username:" << event->get_username() << " pwd:" << event->get_pwd();
    User* user = UserDao::selectOneByUsernameAndPassword(event->get_username(),event->get_pwd());
    if(user == nullptr)
    {
        return new LoginResponse(false,"login failed");
    }
    LoginResponse* response = new LoginResponse(true,"",*user);
    return response;
}

UserGroupResponse* UserEventHandler::handleUserGroupRequest(const UserGroupRequest* event)
{
    LOG_INFO << "UserGroupRequest userid:" << event->get_userid();
    std::vector<std::shared_ptr<UserGroup>> userGroups = UserGroupDao::getUserGroupByUserId(event->get_userid());
    return new UserGroupResponse(userGroups);
}

UserListResponse* UserEventHandler::handleUserListRequest(const UserListRequest* event)
{
    LOG_INFO << "UserListRequest userid:" << event->get_userid();
    std::vector<FriendshipDTO> friends = UserDao::getFriendList(event->get_userid());
    return new UserListResponse(friends);
}


GroupListResponse* UserEventHandler::handleGroupListRequest(const GroupListRequest* event)
{
    LOG_INFO << "GroupListRequest userid:" << event->get_userid();
    std::vector<Group> groups = UserDao::getGroupList(event->get_userid());
    return new GroupListResponse(groups);
}

GroupMembersResponse* UserEventHandler::handleGroupMembersRequest(const GroupMembersRequest* event)
{
    LOG_INFO << "GroupMembersRequest groupid:" << event->get_groupid();
    GroupDTO group = UserDao::getGroupInfo(event->get_groupid());
    return new GroupMembersResponse(group);
}

MessageSendResponse* UserEventHandler::handleMessageSendRequest(const MessageSendRequest* event)
{
    LOG_INFO << "MessageSendRequest userid:" << event->get_message().getSenderId() << " to:" << event->get_message().getReceiverId();
    //转发
    //获取连接conn
    //私聊
    Message message = event->get_message();
    //获得当前时间字符串string
    std::string time = QQServer::getCurrentTime();
    message.setTime(time);
    message.setStatus("unread");
    if(!MessageDao::insert(message))
    {
       LOG_INFO << "insert message failed";
    }
    else
    {
        LOG_INFO << "insert message success";
    }
    
    if (event->get_message().getChatType() == "s")
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        LOG_INFO << g_userIdToConn.size();
        //打印key
        for(auto iter = g_userIdToConn.begin();iter != g_userIdToConn.end();iter++)
        {
            LOG_INFO << iter->first;
        }
        //event->get_message().getSenderId()
        LOG_INFO << event->get_message().getSenderId();
        muduo::net::TcpConnectionPtr conn = g_userIdToConn[event->get_message().getReceiverId()];
        if(conn)
        {
            LOG_INFO << "conn is not nullptr";
            // conn->send(event->get_message().to_json().dump());
            std::string body =  event->get_message().to_json().dump();
            //发送类型EventType
            struct header buffer;
            short eventType = ET_MessageReceive;
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
        }
        else
        {
            LOG_INFO << "conn is nullptr";
        }   
    }
    else
    {
        //获取群成员
        GroupDTO group = UserDao::getGroupInfo(event->get_message().getReceiverId());
        std::vector<User> users = group.get_users();
       
        LOG_INFO << "conn is not nullptr";
        // conn->send(event->get_message().to_json().dump());
        std::string body =  event->get_message().to_json().dump();
        //发送类型EventType
        struct header buffer;
        short eventType = ET_MessageReceive;
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
        
        
        for(User user : users)
        {
            if(user.get_id() == event->get_message().getSenderId())
            {
                continue;
            }
            muduo::net::TcpConnectionPtr conn = g_userIdToConn[user.get_id()];
            if(conn)
            {
                conn->send(message);
            }
        }
    }


    //todo 保存到数据库 
    //todo 补充完Message类信息

    return new MessageSendResponse(true,message);
}

MessageSendResponse* UserEventHandler::handleMessageRead(const MessageReadRequest* event)
{
    LOG_INFO << "MessageReceive userid:" << event->get_message().getReceiverId() << " to:" << event->get_message().getSenderId();
    //更新数据库
    if (event->get_message().getChatType() == "single")
    {
        MessageDao::updateUserMessageStatus(event->get_message().getSenderId(),event->get_message().getReceiverId(),"read");
        return nullptr;
    }
    return nullptr;
}


MessageListResponse* UserEventHandler::handleMessageListRequest(const MessageListRequest* event)
{
    LOG_INFO << "MessageListRequest userid:" << event->get_message().getReceiverId() << " to:" << event->get_message().getSenderId();
    
    if(event->get_message().getChatType() == "single")
    {
        std::vector<Message> messages = MessageDao::getUserMessageList(event->get_message().getSenderId(),event->get_message().getReceiverId());
        return new MessageListResponse(messages,"single",event->get_message().getReceiverId());
    }
    return nullptr;
}
#pragma once
#include "iEventHandler.h"
#include "def_events.h"

class UserEventHandler:public iEventHandler
{
public:
    UserEventHandler();
    ~UserEventHandler();
    iEvent* handle(const iEvent* event) override;
    // iEvent* parseEvent(EventType eventTpye, const muduo::string& message) override;
private:
    LoginResponse* handleLoginRequest(const LoginRequest* event);
    UserGroupResponse* handleUserGroupRequest(const UserGroupRequest* event);
    UserListResponse* handleUserListRequest(const UserListRequest* event);
    GroupListResponse* handleGroupListRequest(const GroupListRequest* event);
    GroupMembersResponse* handleGroupMembersRequest(const GroupMembersRequest* event);
    MessageSendResponse* handleMessageSendRequest(const MessageSendRequest* event);
    //已读消息
    MessageSendResponse* handleMessageRead(const MessageReadRequest* event);
    MessageListResponse* handleMessageListRequest(const MessageListRequest* event);
};


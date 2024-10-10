#pragma once
#include "iEvent.h"
#include <string>
#include "nlohmann/json.hpp"
#include "entity/User.hpp"
#include "entity/UserGroup.hpp"
#include "entity/FriendshipDTO.hpp"
#include "entity/Group.hpp"
#include "entity/GroupDTO.hpp"
#include "entity/Message.hpp"
#include <vector>

/// 登录请求
class LoginRequest : public iEvent
{
public:
    LoginRequest()
        : iEvent(ET_LoginRequest)
    {
        
    }
    LoginRequest(const std::string& username, const std::string& pwd)
        : iEvent(ET_LoginRequest), m_username(username), m_pwd(pwd)
    {
        
    }
    //生成set和get
    std::string get_username()const{return m_username;}
    std::string get_pwd()const{return m_pwd;}
    //nlohmann::json
    bool from_json(const nlohmann::json& j)
    {
        m_username = j["username"];
        m_pwd = j["password"];
        return true;
    }
    nlohmann::json to_json() const
    {
        nlohmann::json j;
        j["username"] = m_username;
        j["password"] = m_pwd;
        return j;
    }

private:
    std::string m_username;
    std::string m_pwd;
};

//登录响应
class LoginResponse : public iEvent
{
public:
    LoginResponse()
        : iEvent(ET_LoginResponse)
    {   
    }
    LoginResponse(bool result, const std::string& reason,const User& user = User())
        : iEvent(ET_LoginResponse), m_result(result), m_reason(reason),m_user(user)
    {
    }
    //生成set和get
    bool get_result()const{return m_result;}
    std::string get_reason()const{return m_reason;}
    User get_user()const{return m_user;}
    //nlohmann::json
    bool from_json(const nlohmann::json& j)
    {
        m_result = j["result"];
        m_reason = j["reason"];
        m_user.from_json(j["user"]);
        return true;
    }

    nlohmann::json to_json() const
    {
        nlohmann::json j;
        j["result"] = m_result;
        j["reason"] = m_reason;
        j["user"] = m_user.to_json();
        return j;
    }
    

private:
    bool m_result;
    std::string m_reason;
    User m_user;
};


class UserGroupRequest : public iEvent
{
    public:
    UserGroupRequest()
        : iEvent(ET_UserGroupRequest)
    {
    }
    UserGroupRequest(int userid)
        : iEvent(ET_UserGroupRequest), m_userid(userid)
    {
    }
    //get
    int get_userid()const{return m_userid;}
    //nlohmann::json
    bool from_json(const nlohmann::json& j)
    {
        m_userid = j["userid"];
        return true;
    }

    nlohmann::json to_json() const
    {
        nlohmann::json j;
        j["userid"] = m_userid;
    }

private:
    int m_userid;
};

class UserGroupResponse : public iEvent
{
    public:
    UserGroupResponse()
        : iEvent(ET_UserGroupResponse)
    {
    }
    UserGroupResponse(const std::vector<std::shared_ptr<UserGroup>>& groups)
        : iEvent(ET_UserGroupResponse), m_groups(groups)
    {
    }
    //m_groups的get
    std::vector<std::shared_ptr<UserGroup>> get_groups()const{return m_groups;}
    //m_groups的set
    void set_groups(std::vector<std::shared_ptr<UserGroup>> groups)
    {
        m_groups = groups;
    }
    
    //json
    bool from_json(const nlohmann::json& j)
    {
        for(auto& item : j["groups"])
        {
            // UserGroup group;
            // group.from_json(item);
            // m_groups.push_back(group);
            //使用智能指针
            UserGroup* group = new UserGroup;
            group->from_json(item);
            m_groups.push_back(std::shared_ptr<UserGroup>(group));
        }
        return true;
    }

    nlohmann::json to_json() const
    {
        nlohmann::json j;
        for(auto& item : m_groups)
        {
            j["groups"].push_back(item->to_json());
        }
        return j;
    }

    
private:
    //好友分组集合
       std::vector<std::shared_ptr<UserGroup>> m_groups;
};


class UserListRequest : public iEvent
{

    public:
    UserListRequest()
        : iEvent(ET_UserListRequest)
    {
    }
    UserListRequest(int userid)
        : iEvent(ET_UserListRequest), m_userid(userid)
    {
    }
    //get
    int get_userid()const{return m_userid;}
    //nlohmann::json
    bool from_json(const nlohmann::json& j)
    {
        m_userid = j["userid"];
        return true;
    }

    nlohmann::json to_json() const
    {
        nlohmann::json j;
        j["userid"] = m_userid;
    }   
private:
    int m_userid;
};

class UserListResponse : public iEvent
{
    public:
    UserListResponse()
        : iEvent(ET_UserListResponse)
    {
    }
    UserListResponse(const std::vector<FriendshipDTO>& friendships)
        : iEvent(ET_UserListResponse), m_friendships(friendships)
    {
    }
    
    //获取std::vector<FriendshipDTO> m_users;
    std::vector<FriendshipDTO> get_friendships()const{return m_friendships;}
    

    //json
    //to_json
    bool from_json(const nlohmann::json& j)
    {
        for(auto& item : j["friendships"])
        {
            FriendshipDTO friendship;
            friendship.from_json(item);
            m_friendships.push_back(friendship);
        }
        return true;
    }
    //to_json
    nlohmann::json to_json() const
    {
        nlohmann::json j;
        j["friendships"] = nlohmann::json::array();
        for(auto& item : m_friendships)
        {
            j["friendships"].push_back(item.to_json());
        }
        return j;
    }

    
private:
    //好友分组集合
    std::vector<FriendshipDTO> m_friendships;
};  
    

class GroupListRequest : public iEvent
{

    public:
    GroupListRequest()
        : iEvent(ET_GroupListRequest)
    {
    }
    GroupListRequest(int userid)
        : iEvent(ET_GroupListRequest), m_userid(userid)
    {
    }
    //get
    int get_userid()const{return m_userid;}
    //nlohmann::json
    bool from_json(const nlohmann::json& j)
    {
        m_userid = j["userid"];
        return true;
    }

    nlohmann::json to_json() const
    {
        nlohmann::json j;
        j["userid"] = m_userid;
    }   
private:
    int m_userid;
};  
    
class GroupListResponse : public iEvent
{
    public:
    GroupListResponse()
        : iEvent(ET_GroupListResponse)
    {
    }

    GroupListResponse(const std::vector<Group>& groups)
        : iEvent(ET_GroupListResponse), m_groups(groups)
    {
    }

    //获取std::vector<Group> m_groups;
    std::vector<Group> get_groups()const{return m_groups;}

    //json
    //to_json
    bool from_json(const nlohmann::json& j)
    {
        for(auto& item : j["groups"])
        {
            Group group;
            group.from_json(item);
            m_groups.push_back(group);
        }
        return true;
    }
    
    nlohmann::json to_json() const
    {
        nlohmann::json j;
        j["groups"] = nlohmann::json::array();
        for(auto& item : m_groups)
        {
            j["groups"].push_back(item.to_json());
        }
        return j;
    }

private:
    std::vector<Group>  m_groups;

};

class GroupMembersRequest : public iEvent
{

    public:
    GroupMembersRequest()
        : iEvent(ET_GroupMembersRequest)
    {
    }
    GroupMembersRequest(int groupid)
        : iEvent(ET_GroupMembersRequest), m_groupid(groupid)
    {
    }
    //get
    int get_groupid()const{return m_groupid;}
    //nlohmann::json
    bool from_json(const nlohmann::json& j)
    {
        m_groupid = j["groupid"];
        return true;
    }
    nlohmann::json to_json() const
    {
        nlohmann::json j;
        j["groupid"] = m_groupid;
    }
private:
    int m_groupid;
};

class GroupMembersResponse : public iEvent
{
    public:
    GroupMembersResponse()
        : iEvent(ET_GroupMembersResponse)
    {
    }

    GroupMembersResponse(const GroupDTO& group)
        : iEvent(ET_GroupMembersResponse), group(group)
    {
    }

    GroupDTO get_group()const{return group;}

    bool from_json(const nlohmann::json& j)
    {
        group.from_json(j);
        return true;
    }

    nlohmann::json to_json() const
    {
        return group.to_json();
    }


private:
    GroupDTO group;
};

class MessageSendRequest : public iEvent
{

    public:
    MessageSendRequest()
        : iEvent(ET_MessageSendRequest)
    {
    }

    MessageSendRequest(const Message& message)
        : iEvent(ET_MessageSendRequest), m_message(message)
    {
    }

    Message get_message()const{return m_message;}


    bool from_json(const nlohmann::json& j)
    {
        m_message.from_json(j);
        return true;
    }

    nlohmann::json to_json() const
    {
        return m_message.to_json();
    }

private:
    Message m_message;
};


class MessageSendResponse : public iEvent
{

    public:
    MessageSendResponse()
        : iEvent(ET_MessageSendResponse)
    {
    }

    MessageSendResponse(bool result,const Message& message)
        : iEvent(ET_MessageSendResponse), m_result(result),m_message(message)
    {
    }
    

    bool get_result()const{return m_result;}

    Message get_message()const{return m_message;}


    bool from_json(const nlohmann::json& j)
    {
        m_result = j["result"];
        m_message.from_json(j["message"]);
        return true;
    }

    nlohmann::json to_json() const
    {
        nlohmann::json j;
        j["result"] = m_result;
        j["message"] = m_message.to_json();
        return j;
    }

private:
    bool m_result;
    Message m_message;
};


class MessageReadRequest : public iEvent
{

    public:
    MessageReadRequest()
        : iEvent(ET_MessageRead)
    {
    }

    MessageReadRequest(const Message& message)
        : iEvent(ET_MessageRead), m_message(message)
    {
    }

    Message get_message()const{return m_message;}


    bool from_json(const nlohmann::json& j)
    {
        m_message.from_json(j);
        return true;
    }

    nlohmann::json to_json() const
    {
        return m_message.to_json();
    }

private:
    Message m_message;
};


class MessageListRequest : public iEvent
{

    public:
    MessageListRequest()
        : iEvent(ET_MessageListRequest)
    {
    }

    MessageListRequest(const Message& message)
        : iEvent(ET_MessageListRequest), m_message(message)
    {
    }

    Message get_message()const{return m_message;}

    bool from_json(const nlohmann::json& j)
    {
        m_message.from_json(j);
        return true;
    }
    
    nlohmann::json to_json() const
    {
        return m_message.to_json();
    }   
    
private:
    Message m_message;
};

class MessageListResponse : public iEvent
{

    public:
    MessageListResponse()
        : iEvent(ET_MessageListResponse)
    {
    }
    MessageListResponse(const std::vector<Message>& messages,const std::string chat_type, const int receiver_id)
        : iEvent(ET_MessageListResponse), m_messages(messages),chat_type(chat_type),receiver_id(receiver_id)
    {
    }

    std::vector<Message> get_messages()const{return m_messages;}

    bool from_json(const nlohmann::json& j)
    {
        chat_type = j["chat_type"];
        receiver_id = j["receiver_id"];
        for(auto& item : j["messages"])
        {
            Message message;
            message.from_json(item);
            m_messages.push_back(message);
        }
        return true;
    }

    nlohmann::json to_json() const
    {
        nlohmann::json j;
        j["messages"] = nlohmann::json::array();
        for(auto& item : m_messages)
        {
            j["messages"].push_back(item.to_json());
        }
        j["chat_type"] = chat_type;
        j["receiver_id"] = receiver_id;
        return j;
    }

    
private:
    std::string chat_type;
    int receiver_id;
    std::vector<Message> m_messages;
};
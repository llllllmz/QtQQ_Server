#pragma once
#include <string>
#include <cstring>
class Friendship
{
public:
    void setFriendshipId(int t_friendshipId){this->friendshipId = t_friendshipId;}
    int getFriendshipId(){return friendshipId;}
    void setUserId(int t_userId){this->userId = t_userId;}
    int getUserId(){return userId;}
    void setFriendId(int t_friendId){this->friendId = t_friendId;}
    int getFriendId(){return friendId;}
    void setUserGroupId(int t_userGroupId){this->userGroupId = t_userGroupId;}
    int getUserGroupId(){return userGroupId;}

    //json
    nlohmann::json to_json() const
    {
        return nlohmann::json
        {
            {"friendshipId", friendshipId},
            {"userId", userId},
            {"friendId", friendId},
            {"userGroupId", userGroupId}
        };
    }

    bool from_json(const nlohmann::json& j)
    {
        friendshipId = j.at("friendshipId").get<int>();
        userId = j.at("userId").get<int>();
        friendId = j.at("friendId").get<int>();
        userGroupId = j.at("userGroupId").get<int>();
        return true;
    }

private:
    int friendshipId;
    int userId;
    int friendId;
    int userGroupId;
};

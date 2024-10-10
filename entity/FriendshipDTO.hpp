#pragma once
#include "Friendship.hpp"
#include "User.hpp"
class FriendshipDTO : public Friendship
{
private:
    /* data */
     User user;
public:
    FriendshipDTO(): user(User()) {}
    ~FriendshipDTO(){}
    
    User getUser()
    {
        return user;
    }
    void setUser(User &user)
    {
        this->user = user;
    }
    bool from_json(const nlohmann::json& j)
    {
        Friendship::from_json(j);
        user.from_json(j["user"]);
        return true;
    };
    nlohmann::json to_json() const
    {
        //先获取friendship的json
        nlohmann::json j = Friendship::to_json();
        j["user"] = user.to_json();
        return j;
    }

};
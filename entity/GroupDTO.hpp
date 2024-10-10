// GroupDTO.h

#pragma once
#include <string>
#include <vector>
#include <../nlohmann/json.hpp>
#include "User.hpp" // 假设 User 类定义在 User.h 中

class GroupDTO : public Group
{
public:
    GroupDTO() {}
    GroupDTO(int id, std::string name, std::string description, const std::vector<User>& users)
        : Group(id, name, description), m_users(users) {}

    // 添加用户列表的访问器
    void add_user(const User& user) { m_users.push_back(user); }
    std::vector<User>& get_users() { return m_users; }

    // JSON 序列化
    nlohmann::json to_json() const
    {
        nlohmann::json j = Group::to_json();
        // j["users"] = m_users;
        j["users"] = nlohmann::json::array();
        for (const auto& user : m_users)
            j["users"].push_back(user.to_json());
        return j;
    }

    // JSON 反序列化
    bool from_json(const nlohmann::json& j)
    {
        if (!Group::from_json(j))
            return false;

        try
        {
            m_users.clear();
            const auto& usersJson = j.at("users");
            for (const auto& userJson : usersJson)
            {
                User user;
                if (!user.from_json(userJson))
                    return false;
                m_users.push_back(user);
            }
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

private:
    std::vector<User> m_users;
};
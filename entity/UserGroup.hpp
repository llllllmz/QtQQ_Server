#pragma once
#include <string>
#include "../nlohmann/json.hpp"
class UserGroup
{
    public:
        UserGroup(){};
        ~UserGroup(){};
        void set_userGroupId(int t_userGroupId){userGroupId = t_userGroupId;}
        int get_userGroupId(){return userGroupId;}
        void set_groupName(std::string t_groupName){groupName = t_groupName;}
        std::string get_groupName(){return groupName;}
        void set_userId(int t_userId){userId = t_userId;}
        int get_userId(){ return userId;}
        void set_sortOrder(int t_sortOrder){sortOrder = t_sortOrder;}
        int get_sortOrder(){ return sortOrder; }

        //转换成json
        nlohmann::json to_json() const
        {
            nlohmann::json j;
            j["userGroupId"] = userGroupId;
            j["groupName"] = groupName;
            j["userId"] = userId;
            j["sortOrder"] = sortOrder;
            return j;
        }

        //从json中解析
        bool from_json(const nlohmann::json& j)
        {
            userGroupId = j.at("userGroupId").get<int>();
            groupName = j.at("groupName").get<std::string>();
            userId = j.at("userId").get<int>();
            sortOrder = j.at("sortOrder").get<int>();
        }

    private:
        int userGroupId;
        std::string groupName;
        int userId;
        int sortOrder;

};

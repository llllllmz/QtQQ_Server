#pragma once
#include "../entity/User.hpp"
#include "../sqlconnection.h"
#include "../entity/Friendship.hpp"
#include "../entity/FriendshipDTO.hpp"
#include "../entity/Group.hpp"
#include "../entity/GroupDTO.hpp"
#include <cstdio>
#include <string>
class UserDao
{
    public:
    // 查询用户名和密码
    static User* selectOneByUsernameAndPassword(const std::string& username, const std::string& password)
    {
        //获取连接
        MysqlConnection conn;
        conn.Init();
        SqlRecordSet recordSet;
        char sql[1024];
        sprintf(sql, "select * from user where username='%s' and password='%s'", username.c_str(), password.c_str());
        conn.Execute(sql, recordSet);
        //获取结果
        if (recordSet.GetRowCount() == 0)
        {
            return NULL;
        } else {
            User* user = new User();
            MYSQL_ROW row = recordSet.FetchRow();
            if (row == NULL)
            {
                return NULL;
            }
            user->set_id(atoi(row[0]));
            user->set_username(row[1]);
            user->set_password(row[2]);
            user->set_signname(row[3]);
            return user;
        }
    }
    
    //获取好友列表
    static std::vector<FriendshipDTO> getFriendList(int userId)
    {
        std::vector<FriendshipDTO> result;
        MysqlConnection conn;
        conn.Init();
        SqlRecordSet recordSet;
        char sql[1024];
        //关联user表得到friendship_id, user_id, friend_id, user_group_id和user表中的user_id、username、signname字段
        sprintf(sql, "select friendship.friendship_id, friendship.user_id, friendship.friend_id, friendship.user_group_id, user.id, user.username, user.signname from friendship, user where friendship.friend_id=user.id and friendship.user_id=%d", userId);
        conn.Execute(sql, recordSet);
        if (recordSet.GetRowCount() == 0)
        {
            return result;
        }
        while (MYSQL_ROW row = recordSet.FetchRow()) {
            FriendshipDTO friendship;
            friendship.setFriendshipId(atoi(row[0]));
            friendship.setUserId(atoi(row[1]));
            friendship.setFriendId(atoi(row[2]));
            friendship.setUserGroupId(atoi(row[3]));
            User user;
            user.set_id(atoi(row[4]));
            user.set_username(row[5]);
            user.set_signname(row[6]);
            friendship.setUser(user);
            
            result.push_back(friendship);
        }
    }

    //getGroupList
    static std::vector<Group> getGroupList(int userId)
    {
        std::vector<Group> result;
        MysqlConnection conn;
        conn.Init();
        SqlRecordSet recordSet;
        char sql[1024];
        // 查询给定 user_id 的所有群组信息
        sprintf(sql, "SELECT g.group_id, g.group_name, g.description "
                 "FROM group_members gm "
                 "JOIN groups g ON gm.group_id = g.group_id "
                 "WHERE gm.user_id = %d", userId);
        conn.Execute(sql, recordSet);
        if (recordSet.GetRowCount() == 0)
        {
            return result;
        }
        while (MYSQL_ROW row = recordSet.FetchRow()) {
            Group group;
            group.set_id(atoi(row[0]));
            group.set_name(row[1]);
            group.set_description(row[2]);

            result.push_back(group);            
        }        
    }

    // 获取群成员列表
    static GroupDTO getGroupInfo(int groupId)
    {
        GroupDTO group;
        MysqlConnection conn;
        conn.Init();
        SqlRecordSet recordSet;
        char sql[1024];
        // 查询给定 group_id 的所有群成员信息,
        sprintf(sql, "SELECT g.group_id, g.group_name, g.description, u.id, u.username, u.signname "
                 "FROM group_members gm "
                 "JOIN groups g ON gm.group_id = g.group_id "
                 "JOIN user u ON gm.user_id = u.id "
                 "WHERE g.group_id = %d", groupId);
                 
        conn.Execute(sql, recordSet);
        if (recordSet.GetRowCount() == 0)
        {
            return group;
        }
        while (MYSQL_ROW row = recordSet.FetchRow()) {
            User user;
            group.set_id(atoi(row[0]));
            group.set_name(row[1]);
            group.set_description(row[2]);
            user.set_id(atoi(row[3]));
            user.set_username(row[4]);
            user.set_signname(row[5]);
            group.add_user(user);
        }

        
        return group;
    }

    
};

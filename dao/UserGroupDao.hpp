//防止多次包含
#pragma once
#include "../entity/UserGroup.hpp"
#include "../sqlconnection.h"
#include<vector>
#include<memory>
class UserGroupDao
{
private:
    /* data */
public:
    UserGroupDao(/* args */);
    ~UserGroupDao();
    // 根据用户id获取用户组
    static std::vector<std::shared_ptr<UserGroup>> getUserGroupByUserId(int userId);
    // 修改用户组
    bool update(UserGroup& userGroup);
    // 插入用户组
    bool insert(UserGroup& userGroup);
    // 删除用户组
    bool deleteUserGroup(int userGroupId);
    // 根据用户组id获取用户组
    UserGroup getUserGroupById(int userGroupId);
};

UserGroupDao::UserGroupDao(/* args */)
{
}

UserGroupDao::~UserGroupDao()
{
}
std::vector<std::shared_ptr<UserGroup>> UserGroupDao::getUserGroupByUserId(int userId)
{

    //获取连接
    MysqlConnection conn;
    conn.Init();
    SqlRecordSet recordSet;
    char sql[1024];
    sprintf(sql, "select * from user_group where user_id = %d",userId);
    conn.Execute(sql, recordSet);
    MYSQL_RES *res = recordSet.GetResult();
    
    if(res == nullptr)
    {
        // std::cout<<"获取结果失败!！"<<std::endl;
        //使用log打印
        LOG_INFO << "获取结果失败!";
        return std::vector<std::shared_ptr<UserGroup>>();
    }   
    std::vector<std::shared_ptr<UserGroup>> userGroups;
    while(MYSQL_ROW row = mysql_fetch_row(res))
    {
        UserGroup *userGroup = new UserGroup();
        userGroup->set_userGroupId(atoi(row[0]));
        userGroup->set_groupName(row[1]);
        userGroup->set_userId(atoi(row[2]));
        userGroup->set_sortOrder(atoi(row[3]));
        userGroups.push_back(std::shared_ptr<UserGroup>(userGroup));
    }


    return userGroups;
}

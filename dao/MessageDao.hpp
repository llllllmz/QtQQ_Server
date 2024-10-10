#pragma once
#include "../entity/User.hpp"
#include "../sqlconnection.h"

#include <cstdio>
#include <string>

class MessageDao
{
    public:
    static bool insert(Message& message)
    {
      
        MysqlConnection conn;
        conn.Init();
        SqlRecordSet recordSet;
        char sql[1024];
        sprintf(sql, "insert into message(sender_id, receiver_id, content, chat_type, message_type, status, timestamp) values(%d, %d, '%s', '%s', '%s', '%s', '%s')", message.getSenderId(), message.getReceiverId(), message.getContent().c_str(), message.getChatType().c_str(), message.getMessageType().c_str(), message.getStatus().c_str(), message.getTime().c_str());
        bool ret = conn.Execute(sql);
        int lastId;
        if(ret)
        {
            lastId = conn.GetInsertId();
        }
        message.setMessageId(lastId);
        return ret;
    }

    //消息已读
    

    static bool updateUserMessageStatus(int senderId,int reciverID, std::string status)
    {
        MysqlConnection conn;
        conn.Init();
        char sql[1024];
        sprintf(sql, "update message set status='%s' where sender_id=%d and receiver_id=%d and chat_type = 'single' and status = 'unread'", status.c_str(), senderId, reciverID);
        return conn.Execute(sql);
    }

    static std::vector<Message> getUserMessageList(int senderId, int receiverId)
    {
        MysqlConnection conn;
        conn.Init();
        char sql[1024];
        sprintf(sql, "SELECT `message_id`, `sender_id`, `receiver_id`, `chat_type`, `message_type`, `content`, `status`, `timestamp` from message where (sender_id=%d and receiver_id=%d) or (sender_id=%d and receiver_id=%d) order by timestamp asc", senderId, receiverId, receiverId, senderId);
        SqlRecordSet recordSet;
        conn.Execute(sql, recordSet);
        std::vector<Message> messageList;
        while(MYSQL_ROW row = recordSet.FetchRow())
        {
            Message message;
            message.setMessageId(atoi(row[0]));
            message.setSenderId(atoi(row[1]));
            message.setReceiverId(atoi(row[2]));
            message.setChatType(row[3]);
            message.setMessageType(row[4]);
            message.setContent(row[5]);
            message.setStatus(row[6]);
            message.setTime(row[7]);
            printf("message:%s\n", row[0]);
            messageList.push_back(message);
        }
        return messageList;
    }
};
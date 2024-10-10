#pragma once
#include <string>
#include <../nlohmann/json.hpp>
#include <iostream>
class Message
{
public:
    // 构造函数
    Message() {}
    Message(int messageId, int senderId, int receiverId, std::string content, std::string chatType, std::string messageType, std::string status)
        : messageId(messageId), senderId(senderId), receiverId(receiverId), content(content), chatType(chatType), messageType(messageType), status(status),time(time) {}

    // Setters
    void setMessageId(int t_messageId) { this->messageId = t_messageId; }
    void setSenderId(int t_senderId) { this->senderId = t_senderId; }
    void setReceiverId(int t_receiverId) { this->receiverId = t_receiverId; }
    void setContent(const std::string& t_content) { this->content = t_content; }
    void setChatType(const std::string& t_chatType) { this->chatType = t_chatType; }
    void setMessageType(const std::string& t_messageType) { this->messageType = t_messageType; }
    void setStatus(const std::string& t_status) { this->status = t_status; }
    void setTime(const std::string& t_time) { this->time = t_time; }

    // Getters
    int getMessageId() const { return messageId; }
    int getSenderId() const { return senderId; }
    int getReceiverId() const { return receiverId; }
    const std::string& getContent() const { return content; }
    const std::string& getChatType() const { return chatType; }
    const std::string& getMessageType() const { return messageType; }
    const std::string& getStatus() const { return status; }
    const std::string& getTime() const { return time; }

    // JSON 序列化
    nlohmann::json to_json() const
    {
        return nlohmann::json
        {
            {"messageId", messageId},
            {"senderId", senderId},
            {"receiverId", receiverId},
            {"chatType", chatType},
            {"messageType", messageType},
            {"content", content},
            {"status", status},
            {"time", time}
        };
    }

    // JSON 反序列化
    bool from_json(const nlohmann::json& j)
    {
        try
        {
            messageId = j.at("messageId").get<int>();
            senderId = j.at("senderId").get<int>();
            receiverId = j.at("receiverId").get<int>();
            chatType = j.at("chatType").get<std::string>();
            messageType = j.at("messageType").get<std::string>();
            content = j.at("content").get<std::string>();
            status = j.at("status").get<std::string>();
            time = j.at("time").get<std::string>();
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            return false;
        }
        return true;
    }

private:
    int messageId;
    int senderId;
    int receiverId;
    std::string chatType;
    std::string messageType;
    std::string content;
    std::string status;
    //时间
    std::string time;
};
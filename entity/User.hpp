#pragma once
#include <string>
#include "../nlohmann/json.hpp"
class User
{
    public:
        User() {}
        User(int id, std::string username, std::string password, std::string signname)
            : m_id(id), m_username(username), m_password(password), m_signname(signname) {}

        //set
        void set_id(int id) { m_id = id; }
        void set_username(const std::string& username){ m_username = username; }
        void set_password(const std::string& password){ m_password = password; }
        void set_signname(const std::string& signname){ m_signname = signname; }

        //get
        int get_id() const { return m_id; }
        std::string get_username() const { return m_username; }
        std::string get_password() const { return m_password; }
        std::string get_signname() const { return m_signname; }
        //json
        nlohmann::json to_json() const
        {
            nlohmann::json j;
            j["id"] = m_id;
            j["username"] = m_username;
            j["password"] = m_password;
            j["signname"] = m_signname;
            return j;
        }
        //fromjson
        bool from_json(const nlohmann::json& j)
        {
            try
            {
                m_id = j["id"];
                m_username = j["username"];
                m_password = j["password"];
                m_signname = j["signname"];
                return true;
            }
            catch(...)
            {
                return false;
            }
        }

    private:
        int m_id;
        std::string m_username;
        std::string m_password;
        std::string m_signname;
};
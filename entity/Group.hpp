#pragma once
#include <string>
#include "../nlohmann/json.hpp"

class Group
{
public:
    Group() {}
    Group(int id, std::string name, std::string description)
        : m_id(id), m_name(name), m_description(description) {}

    // Setters
    void set_id(int id) { m_id = id; }
    void set_name(const std::string& name) { m_name = name; }
    void set_description(const std::string& description) { m_description = description; }

    // Getters
    int get_id() const { return m_id; }
    std::string get_name() const { return m_name; }
    std::string get_description() const { return m_description; }

    // JSON serialization
    nlohmann::json to_json() const
    {
        nlohmann::json j;
        j["id"] = m_id;
        j["name"] = m_name;
        j["description"] = m_description;
        return j;
    }

    // JSON deserialization
    bool from_json(const nlohmann::json& j)
    {
        try
        {
            m_id = j.at("id").get<int>();
            m_name = j.at("name").get<std::string>();
            m_description = j.at("description").get<std::string>();
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

private:
    int m_id;
    std::string m_name;
    std::string m_description;
};
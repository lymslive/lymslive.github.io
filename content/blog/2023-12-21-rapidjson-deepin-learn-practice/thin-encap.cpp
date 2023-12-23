#include "rapidjson/document.h"

void Foo(rapidjson::Value& json)
{
    if (json.IsInt())
    {
        int i = json.GetInt();
    }
    if (json.IsString())
    {
        const char* s = json.GetString();
    }

    int index = 4;
    if (json.IsArray() && json.Size() > index)
    {
        auto& node = json[index];
    }

    if (json.IsObject() && json.HasMember("key"))
    {
        auto& node = json["key"];
    }

    auto it = json.FindMember("key");
    if (it != json.MemberEnd())
    {
        auto& node = it->value;
    }
}

int GetValue(rapidjson::Value& json, int def = 0)
{
    if (json.IsInt())
    {
        return json.GetInt();
    }
    return def;
}

bool GetValue(rapidjson::Value& json, int& out)
{
    if (json.IsInt())
    {
        out = json. GetInt();
        return true;
    }
    return false;
}

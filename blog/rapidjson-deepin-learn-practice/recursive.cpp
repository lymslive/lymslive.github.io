#include "rapidjson/document.h"

void deal_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator)
{
    if (json.IsInt())
    {
        printf("int: %d\n", json.GetInt());
    }
    else if (json.IsString())
    {
        printf("string: %s\n", json.GetString());
    }
    else if (json.IsArray())
    {
        printf("array: %d\n", json.Size());
        for (auto it = json.Begin(); it != json.End(); ++it)
        {
            deal_json(*it, allocator);
        }
    }
    else if (json.IsObject())
    {
        printf("object: %d\n", json.MemberCount());
        for (auto it = json.MemberBegin(); it != json.MemberEnd(); ++it)
        {
            deal_json(it->value, allocator);
        }
    }
}

int main()
{
    rapidjson::Document doc;
    auto& allocator = doc.GetAllocator();

    deal_json(doc, allocator);
}

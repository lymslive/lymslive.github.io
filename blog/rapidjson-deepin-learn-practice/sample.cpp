#include "rapidjson/document.h"
#include <iostream>
#include "rapidjson/writer.h"
#include "rapidjson/ostreamwrapper.h"

int main()
{
    rapidjson::Document doc;
    auto& allocator = doc.GetAllocator();
    doc.SetObject();

    rapidjson::Value node;
    node = 123;
    node = 3.14;
    node = "hello rapidjson";
    const char* psz = "hello rapidjson";
    assert(node.GetString() == psz);

    std::string str = "rapijson";
    node.SetString(str.c_str(), str.size(), allocator);
    assert(node.GetString() == reinterpret_cast<const char*>(&node));

    str = "hello rapijson";
    node.SetString(str.c_str(), str.size(), allocator);
    assert(node.GetString() != str.c_str());

    node.SetObject();
    node.AddMember("key", "value", allocator);
    node.AddMember("key", 3.14159, allocator);
    {
        rapidjson::Value key;
        key = "moreKey";
        rapidjson::Value val;
        val = 314159;
        node.AddMember(key, val, allocator);
        assert(key.IsNull());
        assert(val.IsNull());
    }

    rapidjson::Value object;
    object = node;
    assert(node.IsNull());
    assert(object.IsObject());
    doc.AddMember("object", object, allocator);
    assert(object.IsNull());

    auto& refObj = doc["object"];

    rapidjson::OStreamWrapper os(std::cout);
    rapidjson::Writer<rapidjson::OStreamWrapper> writer(os);
    doc.Accept(writer);
}

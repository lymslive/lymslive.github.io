#include "rapidjson/document.h"
#include <iostream>
#include "rapidjson/writer.h"
#include "rapidjson/ostreamwrapper.h"

void Foo()
{
    rapidjson::Value json;
    {
        rapidjson::Document doc;
        auto& allocator = doc.GetAllocator();

        rapidjson::Value node;
        node.SetObject();
        node.AddMember("key1", 1, allocator);
        node.AddMember("key2", 2, allocator);

        json = node;
    }

    auto& node = json["key1"];
}

void Transform()
{
    const char* jsonText = R"({
  "number": "123",
  "array": "[1,2,3]",
  "object": "{\"key\":\"value\"}"
}
)";

    rapidjson::Document doc;
    auto& allocator = doc.GetAllocator();
    doc.Parse(jsonText);
    assert(!doc.HasParseError());

    auto& array = doc["array"];
    if (array.IsString())
    {
        // rapidjson::Document sub;
        rapidjson::Document sub(&allocator);
        sub.Parse(array.GetString());
        if (!sub.HasParseError())
        {
            array = sub.Move();
        }
    }

    auto& object = doc["object"];
    if (object.IsString())
    {
        // rapidjson::Document sub;
        rapidjson::Document sub(&allocator);
        sub.Parse(object.GetString());
        if (!sub.HasParseError())
        {
            object = sub.Move();
        }
    }

    rapidjson::OStreamWrapper os(std::cout);
    rapidjson::Writer<rapidjson::OStreamWrapper> writer(os);
    doc.Accept(writer);
}

int main()
{
    Transform();
}

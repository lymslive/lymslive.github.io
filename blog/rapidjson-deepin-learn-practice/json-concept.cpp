#include <string>
#include <vector>
#include <map>
#include <stdio.h>

using std::string;
using std::vector;
using std::map;

struct json_t {
    union JsonValue {
        union Scalar {
            union Number {
                int i;
                uint32_t u;
                int64_t l;
                uint64_t ul;
                double d;
                float f;
            } num_;
            bool b;
            string* str;
        } scalar_;
        union Polymer {
            vector<json_t>* array;
            map<string, json_t>* object;
        } polymer_;
        void* null_;
    } value_;

    enum JsonType {
        JSON_NUMBER, JSON_STRING,
        JSON_ARRAY, JSON_OBJECT,
        JSON_BOOL, JSON_NULL,
    } type_;
};

int main()
{
    printf("sizeof jsont_t = %ld\n", sizeof(json_t));
}

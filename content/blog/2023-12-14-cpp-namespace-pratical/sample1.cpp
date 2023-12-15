#include <stdio.h>

namespace math
{
    int value_ = 1;
    int GetValue() { return value_; }
    void SetValue(int value) { value_ = value; }
}

namespace physic
{
    int value_ = 2;
    int GetValue() { return value_; }
    void SetValue(int value) { value_ = value; }
}

int value_ = 0;

int main (int argc, char* argv[])
{
    math::SetValue(111);
    physic::SetValue(222);
    int value_ = 333;
    printf("value in math: %d\n", math::GetValue());
    printf("value in physic: %d\n", physic::GetValue());
    printf("value in local: %d\n", value_);
    printf("value in global: %d\n", ::value_);
    return 0;
}

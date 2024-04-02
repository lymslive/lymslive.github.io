struct Circle
{
    double x;
    double y;
    double r;

    static double PI;

    typedef Circle SelfType;

    double Area();
    SelfType& resize(double a_r);
    SelfType& operator=(const SelfType& that);

    // namespace 不允许
    struct impl
    {
        double area(double r)
        {
            return PI * r * r;
        }
    };
};

double Circle::PI = 3.14159;

double Circle::Area()
{
  return PI * r * r;
}


// Circle::SelfType& Circle::resize(double a_r)
// {
//     r = a_r;
//     return *this;
// }

auto Circle::resize(double a_r) -> SelfType&
{
    r = a_r;
    return *this;
}

auto Circle::operator=(const SelfType& that) -> SelfType&
{
    this->x = that.x;
    this->y = that.y;
    this->r = that.r;
    return *this;
}

namespace math
{
    int value_ = 1;
    int GetValue();
    void SetValue(int value);
}

int math::GetValue()
{
    return value_;
}

void math::SetValue(int value)
{
    value_ = value;
}

#include <iostream>

int main()
{
    Circle c;
    c.r = 2.0;
    c.resize(3.0);
    double s = Circle::PI * c.r * c.r;
    std::cout << "area: " << s << std::endl;

    Circle c2;
    c2 = c;
    std::cout << "area: " << c2.Area() << std::endl;

    math::SetValue(111);
    std::cout << "value in math" << math::GetValue() << std::endl;
}

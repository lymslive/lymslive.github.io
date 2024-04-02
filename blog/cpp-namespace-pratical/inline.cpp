
namespace sci { namespace math {
    namespace v1
    {
        double PI = 3.14;
        double area(double r) { return PI * r * r;}
    }
    inline namespace v2
    {
        double PI = 3.14159;
        double area(double r) { return PI * r * r;}
    }
    struct Circle
    {
        double x;
        double y;
        double r;
        double Area() { return area(this->r); }
        static double Area(double r) { return area(r); }
    };
}}

#include <iostream>
int main()
{
    sci::math::Circle c;
    c.r = 2.0;
    std::cout << "c.Area() = " << c.Area() << std::endl;
    std::cout << "v1 area = " << sci::math::v1::area(c.r) << std::endl;
    std::cout << "v2 area = " << sci::math::v2::area(c.r) << std::endl;
    std::cout << "v? area = " << sci::math::area(c.r) << std::endl;
}

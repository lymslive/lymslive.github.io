namespace // static
{
    double area(double r) { return 3.14159 * r * r;}
}

namespace sci { namespace math {
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
    std::cout << "Circle::Area(3.0) = " << sci::math::Circle::Area(3.0) << std::endl;
    std::cout << "c.Area(3.0) = " << c.Area(3.0) << std::endl;
}

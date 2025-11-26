
namespace sci
{
    namespace math
    {
        double PI = 3.14159;

        struct Circle
        {
            double x;
            double y;
            double r;
            // double Area() { return PI * r * r; }
            // static double Area(double r) { return PI * r * r; }
            struct Impl
            {
                static double Area(double r) { return PI * r * r;}
            };
            double Area() { return Impl::Area(r); }
            static double Area(double r) { return Impl::Area(r); }
        };
    }

    namespace phsyic
    {
        double G = 9.80;
        // ...
    }
}

#include <iostream>
int main()
{
    sci::math::Circle c;
    c.r = 2.0;
    std::cout << "c.Area() = " << c.Area() << std::endl;
    std::cout << "Circle::Area(3.0) = " << sci::math::Circle::Area(3.0) << std::endl;
    std::cout << "c.Area(3.0) = " << c.Area(3.0) << std::endl;
}

namespace parent
{
    enum { FALSE, TRUE };
    enum InLine { ZERO, ONE, TWO };
    enum class LoveColor { RED, GREEN, BLUE };
    enum class HateColor { RED, GREEN, BLUE };
}

#include <iostream>
int main()
{
    // int color = parent::LoveColor::BLUE;
    parent::LoveColor color = parent::LoveColor::BLUE;
    std::cout << "color = " << static_cast<int>(color) << std::endl;
    std::cout << "true = " << parent::TRUE << std::endl;
    std::cout << "one = " << parent::ONE << std::endl;
}

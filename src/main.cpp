
#include "base.h"

#include <iostream>

void
printShape(const Shape& shape)
{
    int  w = shape.Shape_wide();
    int  h = shape.Shape_high();
    int* b = shape.Shape_buffer();

    std::cout << std::endl;
    for(int i = 0; i < h; i++)
    {
        for(int j = 0; j < w; j++)
        {
            std::cout << b[i * w + j] << " ";
        }
        std::cout << std::endl;
    }
}

int
main()
{
    std::cout << "test" << std::endl;

    Shape sA;
    sA.Shape_reset(10, 10, 3);

    int* buffer = sA.Shape_buffer();
    printShape(sA);

    Shape sB;
    sB.Shape_reset(5, 5, 2);

    int* bufferB = sB.Shape_buffer();
    printShape(sB);

    int DELTA = 2;

    std::function<void(int&, int&)> func = [&DELTA](int& a, int& b) {
        a = a + b + DELTA;
    };

    std::function<void(int&, int&)> func_ = [&DELTA](int& a, int& b) {
        a = a - b - DELTA;
    };

    printShape(sA);

    int dir[2] = { 0, 0 };

    char ch = '0';
    do
    {
        switch(ch)
        {
            case 'w':
            {
                dir[1]--;
            }
            break;

            case 's':
            {
                dir[1]++;
            }
            break;

            case 'a':
            {
                dir[0]--;
            }
            break;

            case 'd':
            {
                dir[0]++;
            }
            break;

            default:
            {
            }
            break;
        }

        sA.Shape_merge(&sB, func, dir[0], dir[1]);
        printShape(sA);
        sA.Shape_merge(&sB, func_, dir[0], dir[1]);

        std::cin >> ch;

    } while(ch != 'q');


    return 0;
}

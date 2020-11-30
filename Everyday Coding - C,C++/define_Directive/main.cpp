#include <iostream.h>
#include <conio.h>
#define PI 3.141

void main()
{
    float r, area;
    clrscr();
    cout << "Enter radius: ";
    cin >> r;

    area = 2.0*PI*r;
    cout << "Area = " << area;

    getch();
}

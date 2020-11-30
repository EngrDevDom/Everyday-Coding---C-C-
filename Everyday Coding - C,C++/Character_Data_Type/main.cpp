#include <conio.h>
#include <iostream.h>

void main()
{
    clrscr();

    short testVar = 32767;
    cout << testVar << endl;

    testVar = testVar + 1;
    cout << testVar << endl;

    testVar = testVar - 1;
    cout << testVar << endl;

    getch();
}

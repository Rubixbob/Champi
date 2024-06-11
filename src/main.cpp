#include <iostream>

#include "BiSFinder.h"

using namespace std;

int main()
{
    cout << "Press Enter to start..." << endl;
    char a;
    cin.get(a);

    BiSFinder::Instance().init();
    BiSFinder::Instance().findBiS();

    cout << "Press Enter to close..." << endl;
    cin.get(a);
    return 0;
}

#include "Time.h"

#include <iostream>

using namespace std;
using namespace CLPIM;

int main(void)
{
    cout << Timer::GetCurrentTimeInMS() << endl;
    cout << Timer::GetCurrentTimeInSec() << endl;
    return 0;
}

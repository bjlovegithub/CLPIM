#include "Random.h"

#include <ctime>
#include <cstdlib>

using namespace std;
using namespace CLPIM;

bool Random::mInitFlag = false;

int32 Random::NextInt()
{
    if (!mInitFlag) {
        srand(time(NULL));
        mInitFlag = true;
    }
    return rand();
}

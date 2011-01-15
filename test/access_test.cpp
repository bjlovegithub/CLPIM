#include <unistd.h>

#include <iostream>
using namespace std;

int main(void)
{
    cout << access("/home/xianggen.wang/Desktop/google.xml", R_OK | F_OK) << endl;
    return 0;
}

#include <iostream>
#include <fstream>

using namespace std;

int main(void)
{
    ofstream ofs("tmp", ios::out|ios::binary);
    for (int i = 0; i < 100; ++i)
        ofs << i;
    ofs.close();

    ifstream ifs("tmp", ios::in|ios::binary);
    int i;
    ifs >> i;
    cout << i << endl;

    return 0;
}

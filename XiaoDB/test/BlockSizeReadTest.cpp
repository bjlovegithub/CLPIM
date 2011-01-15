/**
 * This program is used for testing performance issues about file IO.
 * Conclusion:
 *   1. Consider about block size when using Liux RAW file API.
 *   2. Set the number of bytes readed each time to times of 
 *      (blocks of per group). Usually it is 8K.
 */

#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        cout << "Usage: " << argv[0] << " [block size] [file name]" << endl;
        return 1;
    }

    timeval start, end;
    int readCount;
    const char *fileName = argv[2];
    size_t blockSize = atoi(argv[1]);
    char buf[blockSize*2];
    int fd = open(fileName, O_RDONLY);
    cout << "Block size: " << blockSize << endl;

    cout << "Linux API Test" << endl;

    if (fd < 0)
    {
        cout << "Open file error!" << endl;
        return 1;
    }

    gettimeofday(&start, NULL);

    while ((readCount=read(fd, buf, blockSize)));

    gettimeofday(&end, NULL);

    cout << "Time Usage: " << end.tv_usec-start.tv_usec << endl;

    cout << "=====\nC API Test" << endl;
    gettimeofday(&start, NULL);
    ifstream ifs(fileName, ios::binary);
    while (ifs.good()) {
        ifs.read(buf, blockSize);
    }
    gettimeofday(&end, NULL);
    cout << "Time Usage: " << end.tv_usec-start.tv_usec << endl;

    return 0;
}

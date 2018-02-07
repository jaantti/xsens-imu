#include "conio.h"
#include "stdio.h"
#include "xsensImu.h"
#include <iostream>
#include <unistd.h>

using namespace std;

int main(int argc, char const *argv[]) {
  XsensImu imu("/dev/ttyUSB0", 115200);
  XsensImu::ImuData data;
  while (!_kbhit()) {
    data = imu.getData(2);
    cout << "--- Press any key to quit ---" << endl;
    cout << data.toString();
    printf("\033c");
    usleep(10000);
  }
}

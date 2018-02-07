#include "conio.h"
#include "stdio.h"
#include "xsensImu.h"
#include <iostream>
#include <unistd.h>

int main(int argc, char const *argv[]) {
  XsensImu imu("/dev/ttyUSB0", 115200);
  XsensImu::ImuData data;
  while (!_kbhit()) {
    data = imu.getData(2);
    std::cout << "--- Press any key to quit ---" << std::endl;
    std::cout << data.toString();
    printf("\033c");
    usleep(10000);
  }
}

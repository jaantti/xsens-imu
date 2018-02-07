#ifndef XSENSIMU_H_
#define XSENSIMU_H_

#include <xcommunication/enumerateusbdevices.h>
#include <xcommunication/int_xsdatapacket.h>
#include <xcommunication/legacydatapacket.h>
#include <xsens/xsdatapacket.h>
#include <xsens/xsportinfoarray.h>
#include <xsens/xstime.h>

#include "deviceclass.h"

#include <iomanip>
#include <iostream>
#include <list>
#include <numeric>
#include <stdexcept>
#include <string>

class XsensImu {
public:
  struct ImuData {
    uint16_t pCnt;
    double temperature;
    double altitude;
    XsQuaternion quaternion;
    XsVector accel;
    XsVector rateOfTurn;
    XsVector magField;
    XsVector latLon;
    XsVector velocity;
    XsPressure pressure;

    ImuData();

    ImuData operator+(const ImuData &rhs);
    ImuData operator/(const double &n);
    std::string toString();
  };

  XsensImu(std::string port, int baudRate);
  ~XsensImu();

  /*!
   * Gets unfiltered data from IMU.
   * @return ImuData. IMU readings in a ImuData struct.
   */
  ImuData getData();

  /*!
   * Returns filtered data using moving average filter.
   * @param N Filter size
   * @return Filtered readings
   */
  ImuData getData(int N);

private:
  DeviceClass device;
  XsPortInfo mtPort;
  XsPressure pressure;
  XsVector magField;
  std::list<ImuData> imuList;
};

#endif

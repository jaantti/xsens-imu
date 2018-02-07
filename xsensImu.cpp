#include "xsensImu.h"
#include <sstream>
#include <string>

XsensImu::ImuData::ImuData() {
  XsVector vec(3);
  XsVector vec2(2);
  vec.setZero();
  vec2.setZero();
  accel = vec;
  rateOfTurn = vec;
  magField = vec;
  latLon = vec2;
  velocity = vec;
  pCnt = 0;
  temperature = 0;
  altitude = 0;
  quaternion = XsQuaternion();
  gpsData = XsRawGnssPvtData();
  utcTime = XsUtcTime();
}

XsensImu::ImuData XsensImu::ImuData::operator+(const XsensImu::ImuData &rhs) {
  XsensImu::ImuData data;
  double w, x, y, z;
  XsVector v;
  XsPressure p;
  data.pCnt = rhs.pCnt;
  data.temperature = temperature + rhs.temperature;
  data.altitude = altitude + rhs.altitude;
  w = quaternion.w() + rhs.quaternion.w();
  x = quaternion.x() + rhs.quaternion.x();
  y = quaternion.y() + rhs.quaternion.y();
  z = quaternion.z() + rhs.quaternion.z();
  data.quaternion = XsQuaternion(w, x, y, z);
  data.accel = accel + (rhs.accel.empty() ? accel : rhs.accel);
  data.rateOfTurn =
      rateOfTurn + (rhs.rateOfTurn.empty() ? rateOfTurn : rhs.rateOfTurn);
  data.magField = magField + (rhs.magField.empty() ? magField : rhs.magField);
  data.latLon = latLon + (rhs.latLon.empty() ? latLon : rhs.latLon);
  data.velocity = velocity + (rhs.velocity.empty() ? velocity : rhs.velocity);
  p.m_pressure = pressure.m_pressure + rhs.pressure.m_pressure;
  data.pressure = p;
  data.gpsData = rhs.gpsData;
  data.utcTime = rhs.utcTime;

  return data;
}

XsensImu::ImuData XsensImu::ImuData::operator/(const double &n) {
  XsensImu::ImuData data;
  double w, x, y, z;
  XsVector v;
  XsPressure p;
  data.pCnt = pCnt;
  data.temperature = temperature / n;
  data.altitude = altitude / n;
  w = quaternion.w() / n;
  x = quaternion.x() / n;
  y = quaternion.y() / n;
  z = quaternion.z() / n;
  data.quaternion = XsQuaternion(w, x, y, z);
  data.accel = accel / n;
  data.rateOfTurn = rateOfTurn / n;
  data.magField = magField / n;
  data.latLon = latLon / n;
  data.velocity = velocity / n;
  p.m_pressure = pressure.m_pressure / n;
  data.pressure = p;
  data.gpsData = gpsData;
  data.utcTime = utcTime;

  return data;
}

std::string XsensImu::ImuData::toString() {
  std::ostringstream os;
  os << "Packet count: " << pCnt << std::endl;
  os << "Temperature: " << temperature << std::endl;
  os << "Altitude: " << altitude << std::endl;
  os << "Orientation q0:" << quaternion.w() << ", q1:" << quaternion.x()
     << ", q2:" << quaternion.y() << ", q3:" << quaternion.z() << std::endl;
  os << "Acceleration: ";
  for (uint i = 0; i < accel.size(); i++) {
    os << "a" << i << ':' << accel[i] << ", ";
  }
  os << std::endl;
  os << "Rate of turn: ";
  for (uint i = 0; i < rateOfTurn.size(); i++) {
    os << "r" << i << ':' << rateOfTurn[i] << ", ";
  }
  os << std::endl;
  os << "Magnetic field: ";
  for (uint i = 0; i < magField.size(); i++) {
    os << "m" << i << ':' << magField[i] << ", ";
  }
  os << std::endl;
  os << "Latitude and longitude: ";
  for (uint i = 0; i < latLon.size(); i++) {
    os << "p" << i << ':' << latLon[i] << ", ";
  }
  os << std::endl;
  os << "Velocity: ";
  for (uint i = 0; i < velocity.size(); i++) {
    os << "v" << i << ':' << velocity[i] << ", ";
  }
  os << std::endl;
  os << "Pressure: " << pressure.m_pressure << std::endl;

  os << "Time: " << (int)utcTime.m_year << std::endl;
  os << gpsData.toString() << std::endl;

  return os.str();
}

XsensImu::XsensImu(std::string port, int baudRate) {

  // Scan for connected USB devices
  std::cout << "Scanning for USB devices..." << std::endl;
  XsPortInfoArray portInfoArray;
  xsEnumerateUsbDevices(portInfoArray);
  if (portInfoArray.empty()) {
    XsPortInfo portInfo(port, XsBaud::numericToRate(baudRate));
    portInfoArray.push_back(portInfo);
  }

  // Use the first detected device
  mtPort = portInfoArray.at(0);

  // Open the port with the detected device
  std::cout << "Opening port..." << std::endl;
  if (!device.openPort(mtPort))
    throw std::runtime_error("Could not open port. Aborting.");

  // Put the device into configuration mode before configuring the device
  std::cout << "Putting device into configuration mode..." << std::endl;
  if (!device.gotoConfig()) {
    throw std::runtime_error(
        "Could not put device into configuration mode. Aborting.");
  }

  // Request the device Id to check the device type
  mtPort.setDeviceId(device.getDeviceId());

  // Check if we have an MTi / MTx / MTmk4 device
  if (!mtPort.deviceId().isMt9c() && !mtPort.deviceId().isLegacyMtig() &&
      !mtPort.deviceId().isMtMk4() && !mtPort.deviceId().isFmt_X000()) {
    throw std::runtime_error("No MTi / MTx / MTmk4 device found. Aborting.");
  }
  std::cout << "Found a device with id: "
            << mtPort.deviceId().toString().toStdString()
            << " @ port: " << mtPort.portName().toStdString()
            << ", baudrate: " << mtPort.baudrate() << std::endl;

  // Print information about detected MTi / MTx / MTmk4 device
  std::cout << "Device: " << device.getProductCode().toStdString() << " opened."
            << std::endl;

  // Configure the device. Note the differences between MTix and MTmk4
  std::cout << "Configuring the device..." << std::endl;
  if (mtPort.deviceId().isMt9c() || mtPort.deviceId().isLegacyMtig()) {
    // output orientation data
    XsOutputMode outputMode = XOM_Orientation;
    // output orientation data as quaternion
    XsOutputSettings outputSettings = XOS_OrientationMode_Quaternion;

    // set the device configuration
    if (!device.setDeviceMode(outputMode, outputSettings)) {
      throw std::runtime_error("Could not configure MT device. Aborting.");
    }
  } else if (mtPort.deviceId().isMtMk4() || mtPort.deviceId().isFmt_X000()) {
    XsOutputConfiguration quat(XDI_Quaternion, 0xFFFF);
    XsOutputConfiguration acc(XDI_Acceleration, 0xFFFF);
    XsOutputConfiguration rot(XDI_RateOfTurn, 0xFFFF);
    XsOutputConfiguration magf(XDI_MagneticField, 0xFFFF);
    XsOutputConfiguration temp(XDI_Temperature, 0xFFFF);
    XsOutputConfiguration velo(XDI_VelocityXYZ, 0xFFFF);
    XsOutputConfiguration pres(XDI_BaroPressure, 0xFFFF);
    XsOutputConfiguration alt(XDI_AltitudeEllipsoid, 0xFFFF);
    XsOutputConfiguration packetCount(XDI_PacketCounter, 0xFFFF);
    XsOutputConfiguration raw_c(XDI_RawAccGyrMagTemp, 0xFFFF);
    XsOutputConfiguration gpstime(XDI_Itow, 0xFFFF);
    XsOutputConfiguration pos(XDI_LatLon, 0xFFFF);
    XsOutputConfiguration gpsdata(XDI_GnssPvtData, 0x0004);
    XsOutputConfiguration gpsinfo(XDI_GnssSatInfo, 0x0004);


    XsOutputConfigurationArray configArray;

    configArray.push_back(quat);
    configArray.push_back(acc);
    configArray.push_back(rot);
    configArray.push_back(magf);
    configArray.push_back(temp);
    configArray.push_back(velo);
    configArray.push_back(pres);
    configArray.push_back(alt);
    configArray.push_back(packetCount);
    configArray.push_back(gpstime);
    configArray.push_back(pos);
    configArray.push_back(gpsdata);
    configArray.push_back(gpsinfo);


    if (!device.setOutputConfiguration(configArray)) {
      throw std::runtime_error("Could not configure MTmk4 device. Aborting.");
    }
  } else {
    throw std::runtime_error("Unknown device while configuring. Aborting.");
  }

  // Put the device in measurement mode
  std::cout << "Putting device into measurement mode..." << std::endl;
  if (!device.gotoMeasurement()) {
    throw std::runtime_error(
        "Could not put device into measurement mode. Aborting.");
  }

  std::cout << "\nMain loop (press any key to quit)" << std::endl;
  std::cout << std::string(79, '-') << std::endl;
}

XsensImu::~XsensImu() { device.close(); }

XsensImu::ImuData XsensImu::getData() {
  ImuData newData;
  XsByteArray data;
  XsMessageArray msgs;
  device.readDataToBuffer(data);
  device.processBufferedData(data, msgs);
  for (const auto &xmessage : msgs) {
    // Retrieve a packet
    XsDataPacket packet;
    if (xmessage.getMessageId() == XMID_MtData) {
      LegacyDataPacket lpacket(1, false);
      lpacket.setMessage(xmessage);
      lpacket.setXbusSystem(false);
      lpacket.setDeviceId(mtPort.deviceId(), 0);
      lpacket.setDataFormat(
          XOM_Orientation, XOS_OrientationMode_Quaternion, 0);
      XsDataPacket_assignFromLegacyDataPacket(&packet, &lpacket, 0);
    } else if (xmessage.getMessageId() == XMID_MtData2) {
      packet.setMessage(xmessage);
      packet.setDeviceId(mtPort.deviceId());
    }

    // Packet counter
    uint16_t pCnt = packet.packetCounter();

    // Temperature data
    double temp = packet.temperature();

    // Altitude data
    double altitude = packet.altitude();

    // Get the quaternion data
    XsQuaternion quaternion = packet.orientationQuaternion();

    // Get acceleration data
    XsVector accel = packet.calibratedAcceleration();

    // Rate of turn data
    XsVector rateOfTurn = packet.calibratedGyroscopeData() /*rateOfTurnHR()*/;

    // Magnetic field data
    if (packet.containsCalibratedMagneticField())
      magField = packet.calibratedMagneticField();

    // position data
    XsVector latLon = packet.latitudeLongitude();

    // GPS data
    bool hasGps = packet.containsRawGnssPvtData();
    bool hasLatLon = packet.containsLatitudeLongitude();
    auto gpsData = packet.rawGnssPvtData();
    auto utcTime = packet.utcTime();


    // XsVector latLon = packet.positionLLA();

    // Velocity data
    XsVector velocity = packet.velocity();

    // pressure data
    if (packet.containsPressure())
      pressure = packet.pressure();

    newData.pCnt = pCnt;
    newData.temperature = temp;
    newData.altitude = altitude;
    newData.quaternion = quaternion;
    newData.accel = accel;
    newData.rateOfTurn = rateOfTurn;
    newData.magField = magField;
    newData.latLon = latLon;
    newData.velocity = velocity;
    newData.pressure = pressure;
    newData.gpsData = gpsData;
    newData.utcTime = utcTime;

    msgs.clear();
    XsTime::msleep(0);

    return newData;
  }
}

XsensImu::ImuData XsensImu::getData(int N) {
  if (N < 1)
    throw std::string("Filter size must be greater than 1");

  if (imuList.size() >= N)
    imuList.pop_front();

  // If new value is missing, use old one
  XsensImu::ImuData newData = getData();
  if (imuList.size() > 1) {
    XsensImu::ImuData oldData = imuList.front();
    if (newData.accel.empty())
        newData.accel = oldData.accel;
    if (newData.latLon.empty())
        newData.latLon = oldData.latLon;
    if (newData.magField.empty())
        newData.magField = oldData.magField;
    if (newData.rateOfTurn.empty())
        newData.rateOfTurn = oldData.rateOfTurn;
    if (newData.velocity.empty())
        newData.velocity = oldData.velocity;
  }
      
  imuList.push_back(newData);

  XsensImu::ImuData filteredData =
      std::accumulate(imuList.begin(), imuList.end(), XsensImu::ImuData()) /
      imuList.size();
  
  return filteredData;
}

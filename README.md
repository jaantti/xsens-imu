# XSens C++ API

**This software and documentation constitute an unpublished work and is to be considered confidential.**

## Dependencies
Following packages are required for building and running the code:
* gcc/g++ version 5.4 or higher
* CMake version 3.1 or higher
* pthreads

## Building the project
Navigate to project folder

`$ cd /to/project/folder`

Make directory for output and navigate to it

`$ mkdir build`

`$ cd build`

Build the project

`$ cmake ..`

`$ make`

## Running the example
After building there is a executable __example__ in the __build__ directory.

To run the code

`./example`

and you should see something similar

```
--- Press any key to quit ---  
Packet count: 42435  
Temperature: 40.1875  
Altitude: 111.353
Orientation q0:0.994714, q1:0.084209, q2:0.0581912, q3:-0.00813437
Acceleration: a0:-1.17019, a1:1.70133, a2:9.60646,
Rate of turn: r0:0.000844896, r1:0.00778139, r2:0.00366867,
Magnetic field: m0:-0.136947, m1:-1.01819, m2:-1.35945,
Latitude and longitude: p0:58.3659, p1:26.6951,
Velocity: v0:0.0114321, v1:0.0613401, v2:-0.153376,
Pressure: 100145
```

If there are no values for _altitude_, _latitude and longitude_ and _velocity_, then there is no GPS fix. Check the antenna connnection and move the antenna closer to window or use the device outside.

If you get error _Could not open port_, check if device is connected.

Command

`$ ls /dev/ttyU*`

should output

`/dev/ttyUSB0`

If no devices are found, check your connections. Device takes a bit of time after connecting before you can use it.

If you have other devices connected, change the port in __example.cpp__ and rebuild the project.

## Using API
In __example.cpp__ you can see how to access all the data which is published by the IMU.

There are two functions for reading the data:
* __getData()__ - get unfiltered data
* __getData(N)__ - get filtered data. __N__ is the length averaging filter.

## General information about output
|Value|Unit|Max frequency|
|---|---|---|
|Temperature | &#8451; | 1 Hz |
|Altitude | m | 400 Hz |
|Orientation |  [quaternion](https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation) | 400 Hz |
|Acceleration | m/s<sup>2</sup> | 2000 Hz |
|Rate of turn | rad/s | 2000 Hz |
|Magnetic field | Gauss | 50 Hz |
|Latitude and longitude | degrees | 400 Hz |
|Velocity | m/s | 400 Hz |
|Pressure | pascals | 50 Hz |

__Note__

These are the maximum update rates. To set the actual rate, change the values in __xsensImu.cpp:167-179__

![banner](.banners/banner_dark.png#gh-dark-mode-only)
![banner](.banners/banner_light.png#gh-light-mode-only)

# External Micro-ROS Package for RIOT
This external package integrates Micro-ROS with the RIOT operating system.
It allows building and running Micro-ROS applications on RIOT-supported hardware.
Verified to work with RIOT version 2025.04.

## Getting Started
To Use this package, add the following line to your application's makefile:
```makefile
EXTERNAL_PKG_DIRS += <external packages directory>
MICRO_ROS_MODULE ?= <micro ROS directory>

USEPKG += micro_ros
```

The repository must be cloned into a directory named `micro_ros` in the external packages directory; 
other names are not supported.

Examples showing how to set up the Micro-ROS agent and use Micro-ROS with RIOT can be found here: [examples].

[examples]: https://github.com/LeonardHerbst/micro_ros_riot_examples
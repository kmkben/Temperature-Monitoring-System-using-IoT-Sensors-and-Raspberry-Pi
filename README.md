# Temperature Monitoring System using IoT Sensors and Raspberry Pi
## Project Description

This project implements a temperature monitoring system using IoT sensors, Raspberry Pi, LCD display, and LED indicators. It allows users to monitor the temperature remotely and set thresholds for temperature alerts.

## Features

  - Temperature Monitoring: The system reads temperature data from IoT sensors (DHT11) and displays it on an LCD screen.

  - Threshold Alerts: Users can set temperature thresholds. If the temperature exceeds the threshold, an LED indicator starts blinking to alert the user.

  - Client-Server Communication: The system utilizes client-server communication for remote temperature monitoring. Clients can connect to the server and receive real-time temperature updates.

## Prerequisites

  - Raspberry Pi 3 (or compatible)
  - DHT11 IoT Temperature Sensor
  - LCD Display
  - LED Indicators

## Installation
#### Clone the Repository:
```
git clone https://github.com/your-username/temperature-monitoring-system.git
cd temperature-monitoring-system
```

#### Install Dependencies:
Install Qt for the client app.

#### Compile and Run:

## Usage
#### Load the module
`sudo insmod <module_name>`

#### Start the server
The server is C program. So you could just use any compiler C and start it

#### Start Clien app
An app in Qt Widgets, after installing the Qt, use Qt creator to open the project and compile it or you can simply use these command:
##### Generate the Makefile:
`qmake -makefile -o Makefile rpi3sensors_client.pro`
##### Make 
`make`

# FPrime CubeSat Orientation Tracking

Flight software implementation for a simulated CubeSat orientation tracking mission, developed using NASA's F´ (FPrime) framework as part of the Flight Software course at the Aerospace Science and Technology Department, NKUA.

## Mission Overview
The mission simulates a CubeSat continuously tracking its acceleration in three dimensions (X, Y, Z) using an onboard MPU6050 IMU sensor. The system computes 1-second rolling averages of the acceleration data to determine the dominant movement axis and assess the satellite's spatial orientation in real time. Telemetry and events are continuously generated to allow ground operators to monitor the satellite's status, while a three-LED hardware feedback system provides immediate visual indication of sampling activity, power state, and Z-axis orientation alignment. Ground commands allow operators to turn the Payload on or off during operation.

## Components
- **IMU** — Manages I2C communication with the MPU6050 sensor, acquiring 3D acceleration data at 100Hz and transmitting it to other components
- **ImuController** — Controls the IMU component operation and power state
- **ImuData** — Handles and structures the accelerometer data received from the IMU
- **Mpu6050** — Low-level driver for the MPU6050 accelerometer sensor
- **Payload** — Calculates 1-second rolling averages, determines the dominant movement axis, assesses spatial orientation, and manages ground commands (Payload ON/OFF)
- **LedControl** — Manages a three-LED hardware feedback system reflecting sampling status, power state, and Z-axis orientation alignment

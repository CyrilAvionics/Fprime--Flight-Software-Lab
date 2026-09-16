module Components {

  @ Reads real acceleration data from a physical MPU6050 over I2C.
  @ Replaces the simulated Imu/ImuController pair for hardware deployments;
  @ exposes the same ImuDataOut: Components.ImuData port that Payload
  @ already consumes, so Payload requires no changes at all.
  passive component Mpu6050 {

    # ----------------------------------------------------------------------
    # Ports
    # ----------------------------------------------------------------------

    @ Port receiving calls from the rate group (Components-IMU-1/4)
    sync input port run: Svc.Sched

    @ Port sending accel samples to Payload (Components-IMU-5) - same
    @ port type as the simulated Imu component used, for compatibility
    output port ImuDataOut: Components.ImuData

    @ Port for writing register-address bytes to the MPU6050 (Components-IMU-2)
    output port i2cWrite: Drv.I2c

    @ Port for reading register data back from the MPU6050 (Components-IMU-2)
    output port i2cRead: Drv.I2c

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    @ X, Y, Z acceleration from the accelerometer, in g's
    telemetry accelerometer: Acceleration id 0 update always format "{} g"

    @ Result of the most recent I2C transaction (Components-IMU-3)
    telemetry i2cStatus: Drv.I2cStatus id 1 update always format "{}"

    # ----------------------------------------------------------------------
    # Events (Components-IMU-3)
    # ----------------------------------------------------------------------

    @ A write to the MPU6050 failed
    event I2cWriteFailed(status: Drv.I2cStatus) \
      severity warning high \
      format "MPU6050 I2C write failed: {}"

    @ A read from the MPU6050 failed
    event I2cReadFailed(status: Drv.I2cStatus) \
      severity warning high \
      format "MPU6050 I2C read failed: {}"

    @ WHO_AM_I register did not match the expected MPU6050 device ID -
    @ indicates a wiring/address problem even though the bus itself
    @ responded successfully
    event UnexpectedDeviceId(actual: U8) \
      severity warning high \
      format "MPU6050 WHO_AM_I mismatch: got {}"

    @ MPU6050 initialized and responding correctly
    event Mpu6050Ready \
      severity activity high \
      format "MPU6050 initialized and responding correctly"

    # ----------------------------------------------------------------------
    # Standard AC Ports
    # ----------------------------------------------------------------------

    @ Port for requesting the current time
    time get port timeCaller

    @ Port for sending telemetry channels to downlink
    telemetry port tlmOut

    @ Port for sending events to downlink
    event port logOut

    @ Port for sending textual representation of events
    text event port logTextOut

  }

}

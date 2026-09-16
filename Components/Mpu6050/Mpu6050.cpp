// ======================================================================
// \title  Mpu6050.cpp
// \author root
// \brief  cpp file for Mpu6050 component implementation class
// ======================================================================

#include "Components/Mpu6050/Mpu6050.hpp"
#include "FpConfig.hpp"

namespace Components {

  Mpu6050 ::
    Mpu6050(const char* const compName) :
      Mpu6050ComponentBase(compName),
      m_initialized(false)
  {
    this->m_writeBuf[0] = 0;
    for (FwIndexType i = 0; i < 6; i++) {
      this->m_readBuf[i] = 0;
    }
  }

  Mpu6050 ::
    ~Mpu6050()
  {
  }

  void Mpu6050 ::
    run_handler(
        NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    if (!this->m_initialized) {
      this->initSensor();
      this->m_initialized = true;
    }

    this->readAccelerometer();
  }

  void Mpu6050 ::
    initSensor()
  {
    // Components-IMU-1: wake the MPU6050 out of its default sleep mode by
    // writing 0x00 to PWR_MGMT_1. Two-byte write: [register address, value]
    U8 wakeBuf[2] = { REG_PWR_MGMT_1, 0x00 };
    Fw::Buffer wakeWriteBuf(wakeBuf, sizeof(wakeBuf));
    Drv::I2cStatus wakeStatus = this->i2cWrite_out(0, MPU6050_ADDR, wakeWriteBuf);
    this->tlmWrite_i2cStatus(wakeStatus);
    if (wakeStatus != Drv::I2cStatus::I2C_OK) {
      this->log_WARNING_HI_I2cWriteFailed(wakeStatus);
      return;
    }

    // Components-IMU-3: read WHO_AM_I to confirm we're really talking to
    // an MPU6050 at the expected address, not just that the bus is alive
    this->m_writeBuf[0] = REG_WHO_AM_I;
    Fw::Buffer whoAmIWriteBuf(this->m_writeBuf, 1);
    Drv::I2cStatus writeStatus = this->i2cWrite_out(0, MPU6050_ADDR, whoAmIWriteBuf);
    this->tlmWrite_i2cStatus(writeStatus);
    if (writeStatus != Drv::I2cStatus::I2C_OK) {
      this->log_WARNING_HI_I2cWriteFailed(writeStatus);
      return;
    }

    U8 whoAmIBuf[1] = { 0 };
    Fw::Buffer whoAmIReadBuf(whoAmIBuf, 1);
    Drv::I2cStatus readStatus = this->i2cRead_out(0, MPU6050_ADDR, whoAmIReadBuf);
    this->tlmWrite_i2cStatus(readStatus);
    if (readStatus != Drv::I2cStatus::I2C_OK) {
      this->log_WARNING_HI_I2cReadFailed(readStatus);
      return;
    }

    if (whoAmIBuf[0] != EXPECTED_WHO_AM_I) {
      this->log_WARNING_HI_UnexpectedDeviceId(whoAmIBuf[0]);
    } else {
      this->log_ACTIVITY_HI_Mpu6050Ready();
    }
  }

  void Mpu6050 ::
    readAccelerometer()
  {
    // Step 1: tell the MPU6050 which register to start reading from
    this->m_writeBuf[0] = REG_ACCEL_XOUT_H;
    Fw::Buffer writeBuf(this->m_writeBuf, 1);
    Drv::I2cStatus writeStatus = this->i2cWrite_out(0, MPU6050_ADDR, writeBuf);
    this->tlmWrite_i2cStatus(writeStatus);
    if (writeStatus != Drv::I2cStatus::I2C_OK) {
      this->log_WARNING_HI_I2cWriteFailed(writeStatus);
      return;
    }

    // Step 2: read back 6 bytes: X_H, X_L, Y_H, Y_L, Z_H, Z_L
    Fw::Buffer readBuf(this->m_readBuf, 6);
    Drv::I2cStatus readStatus = this->i2cRead_out(0, MPU6050_ADDR, readBuf);
    this->tlmWrite_i2cStatus(readStatus);
    if (readStatus != Drv::I2cStatus::I2C_OK) {
      this->log_WARNING_HI_I2cReadFailed(readStatus);
      return;
    }

    // Combine high/low bytes into signed 16-bit raw values, then scale
    // to g's using the default +/-2g sensitivity (16384 LSB/g)
    const I16 rawX = static_cast<I16>((static_cast<U16>(this->m_readBuf[0]) << 8) | this->m_readBuf[1]);
    const I16 rawY = static_cast<I16>((static_cast<U16>(this->m_readBuf[2]) << 8) | this->m_readBuf[3]);
    const I16 rawZ = static_cast<I16>((static_cast<U16>(this->m_readBuf[4]) << 8) | this->m_readBuf[5]);

    Components::Acceleration accel;
    accel[0] = static_cast<F32>(rawX) / ACCEL_SCALE_LSB_PER_G;
    accel[1] = static_cast<F32>(rawY) / ACCEL_SCALE_LSB_PER_G;
    accel[2] = static_cast<F32>(rawZ) / ACCEL_SCALE_LSB_PER_G;

    this->tlmWrite_accelerometer(accel);
    this->ImuDataOut_out(0, accel);
  }

}

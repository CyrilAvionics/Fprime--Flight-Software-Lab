// ======================================================================
// \title  Mpu6050.hpp
// \author root
// \brief  hpp file for Mpu6050 component implementation class
// ======================================================================

#ifndef Components_Mpu6050_HPP
#define Components_Mpu6050_HPP

#include "Components/Mpu6050/Mpu6050ComponentAc.hpp"

namespace Components {

  class Mpu6050 : public Mpu6050ComponentBase {

    public:

      explicit Mpu6050(const char* const compName);
      ~Mpu6050();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations
      // ----------------------------------------------------------------------

      //! Handler for input port run
      void run_handler(NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context) override;

      // ----------------------------------------------------------------------
      // MPU6050 register map (default I2C address 0x68, AD0 floating)
      // ----------------------------------------------------------------------

      static const U32 MPU6050_ADDR = 0x68;
      static const U8 REG_PWR_MGMT_1 = 0x6B;
      static const U8 REG_WHO_AM_I = 0x75;
      static const U8 REG_ACCEL_XOUT_H = 0x3B;
      static const U8 EXPECTED_WHO_AM_I = 0x68;
      static constexpr F32 ACCEL_SCALE_LSB_PER_G = 16384.0f; // default +/-2g range

      bool m_initialized; //!< true once wake-up + WHO_AM_I check has run

      // Backing storage for I2C transactions - Fw::Buffer wraps these,
      // it doesn't own/allocate its own memory
      U8 m_writeBuf[1];
      U8 m_readBuf[6];

      //! One-time wake-up + WHO_AM_I check, called from the first run_handler
      void initSensor();

      //! Read the 6 accelerometer bytes and publish telemetry + ImuDataOut
      void readAccelerometer();

  };

}

#endif

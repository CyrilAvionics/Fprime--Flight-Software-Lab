// ======================================================================
// \title  IMU.hpp
// \author root
// \brief  hpp file for IMU component implementation class
// ======================================================================

#ifndef Components_IMU_HPP
#define Components_IMU_HPP

#include "Components/IMU/IMUComponentAc.hpp"

#include <chrono>
#include <random>

namespace Components {

  class IMU :
    public IMUComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct IMU object
      IMU(
          const char* const compName //!< The component name
      );

      //! Destroy IMU object
      ~IMU();

      struct AccelData {
         F32 accelX;
         F32 accelY;
         F32 accelZ;
      };


    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for run
      void run_handler(
          NATIVE_INT_TYPE portNum, //!< The port number
          NATIVE_UINT_TYPE context //!< The call order
      );

      // Initialize port data
      Components::Acceleration acc_data;
      // Create struct object
      AccelData a;

      // ----------------------------------------------------------------------
      // IMU Simulated data section
      // ----------------------------------------------------------------------

      Components::Axis dominantAxis = Components::Axis::NegX;
      F32 currentSign = -1.0f;

      F32 dominantValue;
      F32 otherValue;

      std::mt19937 randomGenerator;
      std::uniform_real_distribution<F32> gravityMagnitudeDist;
      std::uniform_real_distribution<F32> noiseDist;

      std::chrono::steady_clock::time_point lastSwitchTime = std::chrono::steady_clock::now();

      // Update accelerometer readings
      void update();

      // Get accelerometer data
      void getAccel(
          AccelData* const data
      );

      //! Update the current sign based on dominantAxis
      void updateSign();


      //! Handler implementation for setAxis
      void setAxis_handler(
          NATIVE_INT_TYPE portNum, //!< The port number
          const Components::Axis& p_Axis
      );

  };

}

#endif

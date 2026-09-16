// ======================================================================
// \title  IMU.cpp
// \author root
// \brief  cpp file for IMU component implementation class
// ======================================================================

#include "Components/IMU/IMU.hpp"
#include "FpConfig.hpp"

namespace Components {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  IMU ::
    IMU(const char* const compName) :
      IMUComponentBase(compName)
        ,a{0.0, 0.0, 0.0},
        randomGenerator(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count())),
        gravityMagnitudeDist(0.9, 1.1),
        noiseDist(-0.1, 0.1)
  {
    updateSign();
  }

  IMU ::
    ~IMU()
  {

  }

  void IMU ::
      update()
  {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastSwitchTime).count();

    dominantValue = gravityMagnitudeDist(randomGenerator) * currentSign;
    otherValue = noiseDist(randomGenerator);

    a.accelX = (dominantAxis == Components::Axis::NegX || dominantAxis == Components::Axis::PosX) ? dominantValue : otherValue;
    a.accelY = (dominantAxis == Components::Axis::NegY || dominantAxis == Components::Axis::PosY) ? dominantValue : otherValue;
    a.accelZ = (dominantAxis == Components::Axis::NegZ || dominantAxis == Components::Axis::PosZ) ? dominantValue : otherValue;
  }

  void IMU ::
    getAccel(
      AccelData *data)
  {
    if (data)
    {
      *data = a;
    }
  }

  void IMU ::
      updateSign()
  {
    currentSign =
      (dominantAxis == Components::Axis::NegX ||
       dominantAxis == Components::Axis::NegY ||
       dominantAxis == Components::Axis::NegZ) ? -1.0f : 1.0f;
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void IMU ::
    run_handler(
        NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    update();
    getAccel(&a);

    // Get accelerometer data
    acc_data[0] = a.accelX;
    acc_data[1] = a.accelY;
    acc_data[2] = a.accelZ;
    this->tlmWrite_accelerometer(acc_data);

    /* Use this later to connect this port with Payload component */
    this->ImuDataOut_out(0, acc_data);
  }

  void IMU ::
    setAxis_handler(
        NATIVE_INT_TYPE portNum,
        const Components::Axis& p_Axis
    )
  {
    dominantAxis = p_Axis;
    updateSign();
  }

}

// ======================================================================
// \title  Payload.hpp
// \author root
// \brief  hpp file for Payload component implementation class
// ======================================================================

#ifndef Components_Payload_HPP
#define Components_Payload_HPP

#include "Components/Payload/PayloadComponentAc.hpp"

namespace Components {

  class Payload : public PayloadComponentBase {

    public:
      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      explicit Payload(const char* const compName);
      ~Payload();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations
      // ----------------------------------------------------------------------

      //! Handler for input port accelIn
      //! p_Acceleration is a 3-element array: [0]=x, [1]=y, [2]=z
      void accelIn_handler(FwIndexType portNum, const Components::Acceleration& p_Acceleration) override;

      //! Handler for command StartPayload
      void StartPayload_cmdHandler(
          FwOpcodeType opCode,
          U32 cmdSeq,
          Fw::On on_off
      ) override;

      // ----------------------------------------------------------------------
      // Internal state
      // ----------------------------------------------------------------------

      //! number of samples in a 1-second window.
      //! Measured from MissionDeploymentTopology.cpp: rateDriver.configure(1)
      //! gives a 1ms base tick, and rateGroupDivisors{{100,0},{1000,0}} gives
      //! rateGroup1 (which drives imu.run -> accelIn) a divisor of 100, i.e.
      //! a 100ms period = 10Hz. So SAMPLE_WINDOW = 10 matches the CURRENT
      //! configuration. If you later change the rateGroup1 divisor (e.g. to
      //! {10,0} for a true 100Hz to match Components-IMU-4), update this to
      //! match: SAMPLE_WINDOW should always equal accelIn's calls-per-second.
      static const FwIndexType SAMPLE_WINDOW = 10;

      F32 m_xBuf[SAMPLE_WINDOW];   //!< circular buffer of last N samples
      F32 m_yBuf[SAMPLE_WINDOW];
      F32 m_zBuf[SAMPLE_WINDOW];

      FwIndexType m_bufIndex;      //!< next slot to write in circular buffer
      FwIndexType m_sampleCount;   //!< samples collected so far (caps at SAMPLE_WINDOW)

      F32 m_sumX;                  //!< running sum over the buffer, for O(1) averaging
      F32 m_sumY;
      F32 m_sumZ;

      bool m_payloadOn;                     //!< true once StartPayload(ON) received
      Components::Axis m_lastDominantAxis; //!< last reported dominant axis (to detect changes)

      //! Push a new accel sample into the rolling window and update running sums
      void pushSample(F32 x, F32 y, F32 z);

      //! Compute dominant axis from the three averages
      Components::Axis computeDominantAxis(F32 avgX, F32 avgY, F32 avgZ) const;

  };

}

#endif
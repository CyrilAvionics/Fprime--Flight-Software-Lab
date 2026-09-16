// ======================================================================
// \title  Payload.cpp
// \author root
// \brief  cpp file for Payload component implementation class
// ======================================================================

#include "Components/Payload/Payload.hpp"
#include "FpConfig.hpp"

namespace Components {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  Payload ::
    Payload(const char* const compName) :
      PayloadComponentBase(compName),
      m_bufIndex(0),
      m_sampleCount(0),
      m_sumX(0.0f),
      m_sumY(0.0f),
      m_sumZ(0.0f),
      m_payloadOn(false),
      m_lastDominantAxis(Components::Axis::PosZ)
  {
    for (FwIndexType i = 0; i < SAMPLE_WINDOW; i++) {
      this->m_xBuf[i] = 0.0f;
      this->m_yBuf[i] = 0.0f;
      this->m_zBuf[i] = 0.0f;
    }
  }

  Payload ::
    ~Payload()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations
  // ----------------------------------------------------------------------

  void Payload ::
    accelIn_handler(
        FwIndexType portNum,
        const Components::Acceleration& p_Acceleration
    )
  {
    // Acceleration is a 3-element array: [0]=x, [1]=y, [2]=z
    const F32 x = p_Acceleration[0];
    const F32 y = p_Acceleration[1];
    const F32 z = p_Acceleration[2];

    // Components-Payload-1: always report the raw sample as telemetry
    this->tlmWrite_AccelX(x);
    this->tlmWrite_AccelY(y);
    this->tlmWrite_AccelZ(z);

    if (!this->m_payloadOn) {
      // payload is off: keep raw telemetry alive, but do nothing else
      return;
    }

    // Components-Payload-6: red LED blinking is enabled/disabled once in
    // StartPayload_cmdHandler (see below), not toggled per-sample here -
    // LedControl's own run() cycle handles the actual toggling now.

    // Components-Payload-2: maintain rolling 1-second average
    this->pushSample(x, y, z);

    if (this->m_sampleCount < SAMPLE_WINDOW) {
      // not enough samples yet to report a full 1-second average
      return;
    }

    const F32 avgX = this->m_sumX / static_cast<F32>(SAMPLE_WINDOW);
    const F32 avgY = this->m_sumY / static_cast<F32>(SAMPLE_WINDOW);
    const F32 avgZ = this->m_sumZ / static_cast<F32>(SAMPLE_WINDOW);

    this->tlmWrite_AvgAccelX(avgX);
    this->tlmWrite_AvgAccelY(avgY);
    this->tlmWrite_AvgAccelZ(avgZ);

    // Components-Payload-3: determine + report dominant axis
    const Components::Axis dominant = this->computeDominantAxis(avgX, avgY, avgZ);
    this->tlmWrite_DominantAxis(dominant);

    if (dominant != this->m_lastDominantAxis) {
      this->log_ACTIVITY_HI_DominantAxisChanged(dominant);
      this->m_lastDominantAxis = dominant;
    }

    // Components-Payload-5: green LED reflects Z-axis dominance (either
    // sign counts - the requirement only cares about the axis, not
    // direction, so PosZ and NegZ both light it)
    if (this->isConnected_greenLedSet_OutputPort(0)) {
      const bool zDominant = (dominant == Components::Axis::PosZ) ||
                              (dominant == Components::Axis::NegZ);
      this->greenLedSet_out(0, zDominant ? Fw::On::ON : Fw::On::OFF);
    }
  }

  void Payload ::
    StartPayload_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::On on_off
    )
  {
    this->m_payloadOn = (on_off == Fw::On::ON);

    // Components-Payload-7: blue LED mirrors payload on/off state
    if (this->isConnected_blueLedSet_OutputPort(0)) {
      this->blueLedSet_out(0, this->m_payloadOn ? Fw::On::ON : Fw::On::OFF);
    }

    // Components-Payload-6: enable/disable red LED blink mode once here -
    // LedControl.setBlinking toggles it internally every run() cycle
    if (this->isConnected_redBlinkEnable_OutputPort(0)) {
      this->redBlinkEnable_out(0, this->m_payloadOn ? Fw::On::ON : Fw::On::OFF);
    }

    if (!this->m_payloadOn) {
      // reset the averaging window and turn off the sampling-dependent LEDs.
      // IMPORTANT: also clear the buffer arrays themselves, not just the
      // sums/index/count - otherwise pushSample() will later subtract
      // stale leftover values from a freshly-zeroed sum on restart,
      // corrupting the average for the first SAMPLE_WINDOW samples.
      for (FwIndexType i = 0; i < SAMPLE_WINDOW; i++) {
        this->m_xBuf[i] = 0.0f;
        this->m_yBuf[i] = 0.0f;
        this->m_zBuf[i] = 0.0f;
      }
      this->m_bufIndex = 0;
      this->m_sampleCount = 0;
      this->m_sumX = 0.0f;
      this->m_sumY = 0.0f;
      this->m_sumZ = 0.0f;
      if (this->isConnected_greenLedSet_OutputPort(0)) {
        this->greenLedSet_out(0, Fw::On::OFF);
      }
    }

    this->log_ACTIVITY_HI_PayloadStateChanged(on_off);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  // ----------------------------------------------------------------------
  // Private helpers
  // ----------------------------------------------------------------------

  void Payload ::
    pushSample(F32 x, F32 y, F32 z)
  {
    // subtract the value about to be overwritten (0 the first time through)
    this->m_sumX -= this->m_xBuf[this->m_bufIndex];
    this->m_sumY -= this->m_yBuf[this->m_bufIndex];
    this->m_sumZ -= this->m_zBuf[this->m_bufIndex];

    this->m_xBuf[this->m_bufIndex] = x;
    this->m_yBuf[this->m_bufIndex] = y;
    this->m_zBuf[this->m_bufIndex] = z;

    this->m_sumX += x;
    this->m_sumY += y;
    this->m_sumZ += z;

    this->m_bufIndex = (this->m_bufIndex + 1) % SAMPLE_WINDOW;

    if (this->m_sampleCount < SAMPLE_WINDOW) {
      this->m_sampleCount++;
    }
  }

  Components::Axis Payload ::
    computeDominantAxis(F32 avgX, F32 avgY, F32 avgZ) const
  {
    const F32 absX = (avgX < 0.0f) ? -avgX : avgX;
    const F32 absY = (avgY < 0.0f) ? -avgY : avgY;
    const F32 absZ = (avgZ < 0.0f) ? -avgZ : avgZ;

    // First pick which axis has the largest magnitude, exactly as before -
    // then use that axis's sign to choose the Pos/Neg variant.
    if (absX >= absY && absX >= absZ) {
      return (avgX >= 0.0f) ? Components::Axis::PosX : Components::Axis::NegX;
    } else if (absY >= absX && absY >= absZ) {
      return (avgY >= 0.0f) ? Components::Axis::PosY : Components::Axis::NegY;
    } else {
      return (avgZ >= 0.0f) ? Components::Axis::PosZ : Components::Axis::NegZ;
    }
  }

}
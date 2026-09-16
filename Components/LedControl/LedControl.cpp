// ======================================================================
// \title  LedControl.cpp
// \author Angelos Motsios
// \brief  cpp file for LedControl component implementation class
// ======================================================================

#include "Components/LedControl/LedControl.hpp"
#include "FpConfig.hpp"

namespace Components {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  LedControl ::
    LedControl(const char* const compName) :
      LedControlComponentBase(compName)
  {

  }

  LedControl ::
    ~LedControl()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void LedControl ::
    run_handler(
        NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    if (this->m_blinkingEnabled) {
      // Guaranteed to actually flip every single run() cycle - no
      // external caller in the loop, so no possibility of an even
      // number of stages between applications cancelling this out.
      this->new_state = (this->state == Fw::On::ON) ? Fw::On::OFF : Fw::On::ON;
    }

    if (this->state != this->new_state) {
      if (this->isConnected_gpioSet_OutputPort(0)) {
        this->gpioSet_out(0, (Fw::On::ON == new_state) ? Fw::Logic::HIGH : Fw::Logic::LOW);
      }

      this->state = new_state;
      this->tlmWrite_led_state(this->state);
    }
  }

  void LedControl ::
    setLed_handler(
        NATIVE_INT_TYPE portNum,
        const Fw::On& state
    )
  {
    // Mirrors TOGGLE_LED_cmdHandler's behavior, minus the command response
    // and the event log (this port can be called at high rate by Payload,
    // e.g. once per accel sample, so we don't want to flood the event log
    // the way a rare ground command would). The actual GPIO write and
    // telemetry update still only happen in run_handler, once per
    // rateGroup2 tick, exactly as they do for the TOGGLE_LED command path.
    if (state.isValid()) {
      this->new_state = state;
    }
  }

  void LedControl ::
    setBlinking_handler(
        NATIVE_INT_TYPE portNum,
        const Fw::On& state
    )
  {
    if (!state.isValid()) {
      return;
    }
    this->m_blinkingEnabled = (state == Fw::On::ON);
    if (!this->m_blinkingEnabled) {
      // ensure it settles OFF rather than freezing mid-blink
      this->new_state = Fw::On::OFF;
    }
  }

  // ----------------------------------------------------------------------
  // Handler implementations for commands
  // ----------------------------------------------------------------------

  void LedControl ::
    TOGGLE_LED_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::On on_off
    )
  {
    // Create a variable to represent the command response
    auto cmdResp = Fw::CmdResponse::OK;

    if (!on_off.isValid()) {
      this->log_WARNING_LO_InvalidLedStateArg(on_off);
      // Update command response with a validation error
      cmdResp = Fw::CmdResponse::VALIDATION_ERROR;
    } else {
      this->new_state = on_off;
      this->log_ACTIVITY_HI_SetLedState(new_state);
    }
    
    this->cmdResponse_out(opCode, cmdSeq, cmdResp);
  }

}
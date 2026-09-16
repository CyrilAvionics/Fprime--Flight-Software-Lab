// ======================================================================
// \title  LedControl.hpp
// \author Angelos Motsios
// \brief  hpp file for LedControl component implementation class
// ======================================================================

#ifndef Components_LedControl_HPP
#define Components_LedControl_HPP

#include "Components/LedControl/LedControlComponentAc.hpp"

namespace Components {

  class LedControl :
    public LedControlComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct LedControl object
      LedControl(
          const char* const compName //!< The component name
      );

      //! Destroy LedControl object
      ~LedControl();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------
      
      //! Handler implementation for run
      //!
      //! Port receiving calls from the rate group
      void run_handler(
          NATIVE_INT_TYPE portNum, //!< The port number
          NATIVE_UINT_TYPE context //!< The call order
      );

      //! Handler implementation for setLed
      //!
      //! Port for other components (e.g. Payload) to directly set LED
      //! state, without going through the command dispatcher
      void setLed_handler(
          NATIVE_INT_TYPE portNum, //!< The port number
          const Fw::On& state //!< the LED state to stage
      ) override;

      //! Handler implementation for setBlinking
      //!
      //! Enables/disables a blink pattern that toggles once per run()
      //! cycle - avoids the cancellation bug from external per-sample
      //! toggling landing on an even multiple of run()'s own cadence
      void setBlinking_handler(
          NATIVE_INT_TYPE portNum, //!< The port number
          const Fw::On& state //!< ON = start blinking, OFF = stop and turn off
      ) override;

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for commands
      // ----------------------------------------------------------------------

      //! Handler implementation for command TOGGLE_LED
      //!
      //! Command to toggle the LED once
      void TOGGLE_LED_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq, //!< The command sequence number
          Fw::On on_off //!< Toggle led
      );

      Fw::On state;     //! Keeps track if LED is on or off
      Fw::On new_state;
      bool m_blinkingEnabled = false; //! Whether run_handler should auto-toggle each cycle
  };

}

#endif
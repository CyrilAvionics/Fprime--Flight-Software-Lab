// ======================================================================
// \title  ImuController.hpp
// \author root
// \brief  hpp file for ImuController component implementation class
// ======================================================================

#ifndef Components_ImuController_HPP
#define Components_ImuController_HPP

#include "Components/ImuController/ImuControllerComponentAc.hpp"

namespace Components {

  class ImuController :
    public ImuControllerComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct ImuController object
      ImuController(
          const char* const compName //!< The component name
      );

      //! Destroy ImuController object
      ~ImuController();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for commands
      // ----------------------------------------------------------------------

      //! Handler implementation for command SET_AXIS
      void SET_AXIS_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq, //!< The command sequence number
          Components::Axis axis
      );
      // Declare the current axis variable !!!
      Components::Axis currentAxis; //!< The currently selected control axis

  };

}

#endif

// ======================================================================
// \title  ImuController.cpp
// \author root
// \brief  cpp file for ImuController component implementation class
// ======================================================================

#include "Components/ImuController/ImuController.hpp"
#include "FpConfig.hpp"

namespace Components {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  ImuController ::
    ImuController(const char* const compName) :
      ImuControllerComponentBase(compName)
  {

  }

  ImuController ::
    ~ImuController()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for commands
  // ----------------------------------------------------------------------

  void ImuController ::
    SET_AXIS_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq,
        Components::Axis axis
    )
  {
    if (!axis.isValid()) {
      this->log_WARNING_LO_InvalidAxis(axis);
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
      return;
    }

    if (currentAxis != axis) {
      this->controlAxis_out(0, axis);
      currentAxis = axis;
      this->log_ACTIVITY_HI_ChangeAxis(axis);
    } else {
      this->log_ACTIVITY_LO_SameAxis();
    }

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

}

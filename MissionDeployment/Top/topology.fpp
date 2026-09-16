module MissionDeployment {

  # ----------------------------------------------------------------------
  # Symbolic constants for port numbers
  # ----------------------------------------------------------------------

    enum Ports_RateGroups {
      rateGroup1
      rateGroup2
    }

    enum Ports_StaticMemory {
      framer
      deframer
      deframing
    }

  topology MissionDeployment {

    # ----------------------------------------------------------------------
    # Instances used in the topology
    # ----------------------------------------------------------------------

    instance cmdDisp
    instance commDriver
    instance deframer
    instance eventLogger
    instance fatalAdapter
    instance fatalHandler
    instance framer
    instance rateDriver
    instance rateGroup1
    instance rateGroup2
    instance rateGroupDriver
    instance staticMemory
    instance systemResources
    instance textLogger
    instance timeHandler
    instance tlmSend

    # GPIO Instances
    instance red_led
    instance green_led
    instance blue_led
    
    instance r_gpioDriver
    instance g_gpioDriver
    instance b_gpioDriver

    instance mpu6050
    instance i2cDriver
    instance payload

    # ----------------------------------------------------------------------
    # Pattern graph specifiers
    # ----------------------------------------------------------------------

    command connections instance cmdDisp

    event connections instance eventLogger

    telemetry connections instance tlmSend

    text event connections instance textLogger

    time connections instance timeHandler

    # ----------------------------------------------------------------------
    # Direct graph specifiers
    # ----------------------------------------------------------------------

    connections RateGroups {
      # Block driver
      rateDriver.CycleOut -> rateGroupDriver.CycleIn

      # Rate group 1
      rateGroupDriver.CycleOut[Ports_RateGroups.rateGroup1] -> rateGroup1.CycleIn
      rateGroup1.RateGroupMemberOut[0] -> commDriver.schedIn
      rateGroup1.RateGroupMemberOut[1] -> tlmSend.Run
      rateGroup1.RateGroupMemberOut[2] -> systemResources.run
      rateGroup1.RateGroupMemberOut[3] -> mpu6050.run

      # Rate group 2
      rateGroupDriver.CycleOut[Ports_RateGroups.rateGroup2] -> rateGroup2.CycleIn
      rateGroup2.RateGroupMemberOut[0] -> red_led.run
      rateGroup2.RateGroupMemberOut[1] -> green_led.run
      rateGroup2.RateGroupMemberOut[2] -> blue_led.run

    }

    connections FaultProtection {
      eventLogger.FatalAnnounce -> fatalHandler.FatalReceive
    }

    connections Downlink {

      tlmSend.PktSend -> framer.comIn
      eventLogger.PktSend -> framer.comIn

      framer.framedAllocate -> staticMemory.bufferAllocate[Ports_StaticMemory.framer]
      framer.framedOut -> commDriver.$send

      commDriver.deallocate -> staticMemory.bufferDeallocate[Ports_StaticMemory.framer]

    }
    
    connections Uplink {

      commDriver.allocate -> staticMemory.bufferAllocate[Ports_StaticMemory.deframer]
      commDriver.$recv -> deframer.framedIn
      deframer.framedDeallocate -> staticMemory.bufferDeallocate[Ports_StaticMemory.deframer]

      deframer.comOut -> cmdDisp.seqCmdBuff
      cmdDisp.seqCmdStatus -> deframer.cmdResponseIn

      deframer.bufferAllocate -> staticMemory.bufferAllocate[Ports_StaticMemory.deframing]
      deframer.bufferDeallocate -> staticMemory.bufferDeallocate[Ports_StaticMemory.deframing]
      
    }

    connections MissionDeployment {
      # Add here connections to user-defined components
      red_led.gpioSet   -> r_gpioDriver.gpioWrite
      green_led.gpioSet -> g_gpioDriver.gpioWrite
      blue_led.gpioSet  -> b_gpioDriver.gpioWrite

      mpu6050.ImuDataOut -> payload.accelIn

      mpu6050.i2cWrite -> i2cDriver.write
      mpu6050.i2cRead -> i2cDriver.read

      payload.greenLedSet -> green_led.setLed
      payload.redBlinkEnable -> red_led.setBlinking
      # payload.redLedSet   -> red_led.setLed
      payload.blueLedSet  -> blue_led.setLed
    }

  }

}

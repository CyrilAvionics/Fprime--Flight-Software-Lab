module Components {

  @ Payload component: consumes Imu acceleration samples, tracks a rolling
  @ 1-second average, determines the dominant axis of motion, and drives
  @ the red_led/green_led/blue_led LedControl instances accordingly.
  passive component Payload {

    # ----------------------------------------------------------------------
    # Data-flow ports
    # ----------------------------------------------------------------------

    @ Input port receiving one accel sample from the Imu component's
    @ ImuDataOut port (Components-Payload-1). Carries a 3-element
    @ Acceleration array: [0]=x, [1]=y, [2]=z.
    sync input port accelIn: Components.ImuData

    # ----------------------------------------------------------------------
    # LED output ports - connect these to red_led.setLed / green_led.setLed /
    # blue_led.setLed (the new port added to LedControl - see
    # LedControl_additions.txt)
    # ----------------------------------------------------------------------

    @ Green LED - ON when Z axis is dominant, OFF otherwise (Payload-5)
    output port greenLedSet: Components.SetLedState

    @ Red LED - toggles with each accel sample while payload is ON,
    @ OFF when payload is OFF (Payload-6). This is a mode-enable port,
    @ not a per-sample toggle - see LedControl.setBlinking. Sending ON
    @ once starts LedControl's own internal blink cadence; OFF stops it.
    output port redBlinkEnable: Components.SetLedState

    @ Blue LED - ON whenever payload is ON, OFF otherwise (Payload-7)
    output port blueLedSet: Components.SetLedState

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    @ Turn payload processing on or off (Payload-4)
    sync command StartPayload(
      on_off: Fw.On
    )

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    @ Latest raw X acceleration sample
    telemetry AccelX: F32 id 0

    @ Latest raw Y acceleration sample
    telemetry AccelY: F32 id 1

    @ Latest raw Z acceleration sample
    telemetry AccelZ: F32 id 2

    @ 1-second rolling average of X acceleration
    telemetry AvgAccelX: F32 id 3

    @ 1-second rolling average of Y acceleration
    telemetry AvgAccelY: F32 id 4

    @ 1-second rolling average of Z acceleration
    telemetry AvgAccelZ: F32 id 5

    @ Current dominant axis of motion
    telemetry DominantAxis: Components.Axis id 6

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    @ Emitted whenever the dominant axis changes (Payload-3)
    event DominantAxisChanged(
      axis: Components.Axis
    ) severity activity high format "Dominant axis changed to {}"

    @ Emitted when the payload is switched on or off
    event PayloadStateChanged(
      on_off: Fw.On
    ) severity activity high format "Payload state set to {}"

    # ----------------------------------------------------------------------
    # Standard AC ports (named to match your project's existing convention,
    # e.g. LedControl.fpp)
    # ----------------------------------------------------------------------

    @ Time get port
    time get port timeCaller

    @ Telemetry port
    telemetry port tlmOut

    @ Command registration port
    command reg port cmdRegOut

    @ Command receive port
    command recv port cmdIn

    @ Command response port
    command resp port cmdResponseOut

    @ Event port
    event port logOut

    @ Text event port
    text event port logTextOut

  }

}

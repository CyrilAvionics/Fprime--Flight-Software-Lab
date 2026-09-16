module Components {
    @ Imu Controller Component
    passive component ImuController {

        # ----------------------------------------------------------------------
        # General Ports
        # ----------------------------------------------------------------------

        output port controlAxis: SetAxis

        # ----------------------------------------------------------------------
        # Special ports (Commands, Events, Time)
        # ----------------------------------------------------------------------

        @ Command receive port
        command recv port cmdIn

        @ Command registration port
        command reg port cmdRegOut

        @ Command response port
        command resp port cmdResponseOut

        @ Event log output port
        event port logOut

        @ Text event log output port
        text event port logTextOut

        @ Time get port (required for events)
        time get port timeCaller

        # ----------------------------------------------------------------------
        # Commands
        # ----------------------------------------------------------------------

        sync command SET_AXIS (
            axis : Axis
        )

        # ----------------------------------------------------------------------
        # Events
        # ----------------------------------------------------------------------

        event ChangeAxis(
            axis: Axis
        ) \
          severity activity high \
          format "Set axis {} as dominant"

        event SameAxis severity activity low \
            format "Same axis was selected (No change)"

        event InvalidAxis(
            axis: Axis
        ) \
          severity warning low \
          format "Invalid axis {} was selected"

    }
}
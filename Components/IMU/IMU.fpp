module Components {
    @ Simulated IMU Component
    passive component IMU {

        # ----------------------------------------------------------------------
        # Ports
        # ----------------------------------------------------------------------

        sync input port run: Svc.Sched

        sync input port setAxis: SetAxis

        output port ImuDataOut : ImuData

        # ----------------------------------------------------------------------
        # Telemtry
        # ----------------------------------------------------------------------

        @ X, Y, Z acceleration from accelerometer
        telemetry accelerometer: Acceleration id 0 update always format "{} g"

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending telemetry channels to downlink
        telemetry port tlmOut

    }
}
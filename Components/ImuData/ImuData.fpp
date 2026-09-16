module Components {

    @ Axis enumeration with sign.
    enum Axis {
        NegX = 0,
        PosX = 1,
        NegY = 2,
        PosY = 3,
        NegZ = 4,
        PosZ = 5
    }

    port SetAxis (
        p_Axis : Axis
    )

    @ Array of accelerometer data
    array Acceleration = [3] F32

    port ImuData (
        p_Acceleration : Acceleration
    )
}

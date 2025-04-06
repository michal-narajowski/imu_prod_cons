#include <cstdint>

typedef struct
{
    float xAcc;        // Acceleration [mg, g=9.81]
    float yAcc;        // Acceleration [mg, g=9.81]
    float zAcc;        // Acceleration [mg, g=9.81]
    uint32_t timestampAcc; // Time stamp of accelerometer measurement
    int32_t xGyro;       // Gyro rate of rotation around x [mDeg/s]
    int32_t yGyro;       // Gyro rate of rotation around y [mDeg/s]
    int32_t zGyro;       // Gyro rate of rotation around z [mDeg/s]
    uint32_t timestampGyro; // Time stamp of gyro measurement
    float xMag;        // Magnetic induction x axis [mGauss]
    float yMag;        // Magnetic induction y axis [mGauss]
    float zMag;        // Magnetic induction z axis [mGauss]
    uint32_t timestampMag; // Time stamp of magnetometer measurement
} __attribute__((packed)) Payload_IMU_t;

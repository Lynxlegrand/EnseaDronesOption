HAL_StatusTypeDef IMU_Init();



#define IMU_I2C_ADDR  0x68 << 1  // Endereço I2C da IMU (shiftado para a função HAL)
#define WHO_AM_I_REG  0x75

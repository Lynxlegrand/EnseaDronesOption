HAL_StatusTypeDef IMU_Init();
HAL_StatusTypeDef IMU_ReadAccelGyro_Converted_All(float dt);



#define IMU_I2C_ADDR  0x68 << 1  // Endereço I2C da IMU (shiftado para a função HAL)
#define WHO_AM_I_REG  0x75

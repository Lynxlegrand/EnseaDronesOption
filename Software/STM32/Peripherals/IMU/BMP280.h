#ifndef BMP280_H
#define BMP280_H

#include <stdint.h>
#include "stm32l4xx_hal.h" // Ensure HAL_StatusTypeDef is recognized

/* BMP280 register definitions */
#define BMP280_ADDR         (0x77 << 1) // Standard I2C address (or 0x77 << 1 if CSB is HIGH)
#define BMP280_REG_TEMP_MSB 0xFA        // Temperature register MSB 
#define BMP280_REG_PRESS_MSB 0xF7       // Pressure register MSB
#define BMP280_REG_CTRL_MEAS 0xF4       // Measurement control
#define BMP280_REG_CONFIG    0xF5       // Configuration
#define BMP280_REG_ID        0xD0       // WHO_AM_I
#define BMP280_ID_VALUE      0x58       // Expected BMP280 ID value
#define BMP280_CALIB_DATA 0x88  // Calibration coefficients addresses

/* Structure to store BMP280 calibration coefficients */
typedef struct {
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;
} BMP280_CalibData;


/* Function prototypes */

/**
 * @brief Initializes the BMP280 sensor.
 * @return HAL_OK if initialization succeeds, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef BMP280_Init(void);

/**
 * @brief Reads temperature and pressure from BMP280.
 * @param temperature Pointer to store the temperature in degrees Celsius.
 * @param pressure Pointer to store the pressure in hPa.
 * @return HAL_OK if the read operation is successful, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef BMP280_Read(float *temperature, float *pressure);

/**
 * @brief Reads a register from the BMP280.
 * @param reg The register address to read.
 * @param data Pointer to store the read data.
 * @return HAL_OK if the operation succeeds, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef BMP280_ReadRegister(uint8_t reg, uint8_t *data);

/**
 * @brief Writes a value to a BMP280 register.
 * @param reg The register address to write to.
 * @param value The value to write.
 * @return HAL_OK if the operation succeeds, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef BMP280_WriteRegister(uint8_t reg, uint8_t value);

/**
 * @brief Reads the factory calibration data from BMP280.
 * @return HAL_OK if calibration data is successfully read, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef BMP280_ReadCalibrationData(void);

/**
 * @brief Compensates the raw temperature value using calibration coefficients.
 * @param adc_T The raw ADC temperature reading.
 * @return The compensated temperature in hundredths of a degree Celsius.
 */
int32_t BMP280_Compensate_T(int32_t adc_T);

/**
 * @brief Compensates the raw pressure value using calibration coefficients.
 * @param adc_P The raw ADC pressure reading.
 * @return The compensated pressure in Pascal.
 */
uint32_t BMP280_Compensate_P(int32_t adc_P);

#endif
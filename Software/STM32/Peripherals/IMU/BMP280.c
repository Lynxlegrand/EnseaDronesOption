#include "BMP280.h"
#include "main.h"  
#include <stdio.h>

/* Private variables */
static BMP280_CalibData calib; // Stores calibration coefficients
static int32_t t_fine;         // Variable required for pressure compensation
extern I2C_HandleTypeDef hi2c1; // I2C handle from main.c

/**
 * @brief Reads a register from BMP280 via I2C.
 * @param reg The register address to read.
 * @param data Pointer to store the read data.
 * @return HAL_OK if the operation succeeds, HAL_ERROR otherwise.
 */
 HAL_StatusTypeDef BMP280_ReadRegister(uint8_t reg, uint8_t *data) {
    return HAL_I2C_Mem_Read(&hi2c1, BMP280_ADDR, reg, 1, data, 1, HAL_MAX_DELAY);
}

/**
 * @brief Writes a value to a BMP280 register via I2C.
 * @param reg The register address to write to.
 * @param value The value to write.
 * @return HAL_OK if the operation succeeds, HAL_ERROR otherwise.
 */
 HAL_StatusTypeDef BMP280_WriteRegister(uint8_t reg, uint8_t value) {
    return HAL_I2C_Mem_Write(&hi2c1, BMP280_ADDR, reg, 1, &value, 1, HAL_MAX_DELAY);
}

/**
 * @brief Reads the factory calibration data from BMP280's EEPROM.
 * @return HAL_OK if calibration data is successfully read, HAL_ERROR otherwise.
 */
 HAL_StatusTypeDef BMP280_ReadCalibrationData(void) {
    uint8_t data[24];
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, BMP280_ADDR, BMP280_CALIB_DATA, 1, data, 24, HAL_MAX_DELAY);
    if (status != HAL_OK) return status; // Return error if reading fails

    /* Convert calibration coefficients */
    calib.dig_T1 = (data[1] << 8) | data[0];
    calib.dig_T2 = (data[3] << 8) | data[2];
    calib.dig_T3 = (data[5] << 8) | data[4];
    calib.dig_P1 = (data[7] << 8) | data[6];
    calib.dig_P2 = (data[9] << 8) | data[8];
    calib.dig_P3 = (data[11] << 8) | data[10];
    calib.dig_P4 = (data[13] << 8) | data[12];
    calib.dig_P5 = (data[15] << 8) | data[14];
    calib.dig_P6 = (data[17] << 8) | data[16];
    calib.dig_P7 = (data[19] << 8) | data[18];
    calib.dig_P8 = (data[21] << 8) | data[20];
    calib.dig_P9 = (data[23] << 8) | data[22];

    return HAL_OK;
}

/**
 * @brief Initializes BMP280 sensor and verifies its ID.
 * @return HAL_OK if initialization succeeds, HAL_ERROR otherwise.
 */
 HAL_StatusTypeDef BMP280_Init(void) {
    uint8_t id;
    if (BMP280_ReadRegister(BMP280_REG_ID, &id) != HAL_OK) {
        return HAL_ERROR;
    }
    if (id != BMP280_ID_VALUE) {
        return HAL_ERROR;
    }

    if (BMP280_ReadCalibrationData() != HAL_OK) {
        return HAL_ERROR;
    }

    /* Configure BMP280 */
    if (BMP280_WriteRegister(BMP280_REG_CTRL_MEAS, 0x27) != HAL_OK) return HAL_ERROR;
    if (BMP280_WriteRegister(BMP280_REG_CONFIG, 0xA0) != HAL_OK) return HAL_ERROR;

    return HAL_OK;
}


/**
 * @brief Reads temperature and pressure from BMP280.
 * @param temperature Pointer to store the temperature in degrees Celsius.
 * @param pressure Pointer to store the pressure in hPa.
 * @return HAL_OK if the read operation is successful, HAL_ERROR otherwise.
 */
 HAL_StatusTypeDef BMP280_Read(float *temperature, float *pressure) {
    uint8_t data[6];
    int32_t adc_temp, adc_press;

    if (HAL_I2C_Mem_Read(&hi2c1, BMP280_ADDR, BMP280_REG_PRESS_MSB, 1, data, 6, HAL_MAX_DELAY) != HAL_OK) {
        return HAL_ERROR;
    }

    /* Convert raw ADC values */
    adc_press = (int32_t)((data[0] << 12) | (data[1] << 4) | (data[2] >> 4));
    adc_temp = (int32_t)((data[3] << 12) | (data[4] << 4) | (data[5] >> 4));

    /* Apply compensation formulas */
    *temperature = BMP280_Compensate_T(adc_temp) / 100.0;
    *pressure = BMP280_Compensate_P(adc_press) / 256000.0;

    return HAL_OK;
}

/**
 * @brief Compensates the raw temperature value using calibration coefficients.
 * @param adc_T The raw ADC temperature reading.
 * @return The compensated temperature in hundredths of a degree Celsius.
 */
 int32_t BMP280_Compensate_T(int32_t adc_T) {
    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t)calib.dig_T1 << 1))) * ((int32_t)calib.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)calib.dig_T1)) * ((adc_T >> 4) - ((int32_t)calib.dig_T1))) >> 12) *
            ((int32_t)calib.dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

/**
 * @brief Compensates the raw pressure value using calibration coefficients.
 * @param adc_P The raw ADC pressure reading.
 * @return The compensated pressure in Pascal.
 */
 uint32_t BMP280_Compensate_P(int32_t adc_P) {
    int64_t var1, var2, p;
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)calib.dig_P3) >> 8) + ((var1 * (int64_t)calib.dig_P2) << 12);
    var1 = ((((int64_t)1 << 47) + var1) * (int64_t)calib.dig_P1) >> 33;

    if (var1 == 0) return 0; // Avoid division by zero

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)calib.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)calib.dig_P7) << 4);

    return (uint32_t)p;
}

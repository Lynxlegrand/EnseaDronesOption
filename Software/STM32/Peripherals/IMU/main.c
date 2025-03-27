/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "string.h"
#include <stdlib.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define IMU_I2C_ADDR  0x68 << 1  // Endereço I2C da IMU (shiftado para a função HAL)
#define WHO_AM_I_REG  0x75
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

uint8_t uart_buffer[50];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
extern I2C_HandleTypeDef hi2c1;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void Delay_ms(uint32_t ms) {
    HAL_Delay(ms);
}

void Send_UART_Message(char *msg) {
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}

//FIRST TRY (CHECKING IF IMU IS AVAILABLE)

uint8_t IMU_ReadRegister(uint16_t reg) {
    uint8_t data=0;
    char msg[30];
    if (HAL_I2C_Mem_Read(&hi2c1, IMU_I2C_ADDR, reg, 1, &data, 1, HAL_MAX_DELAY)!=HAL_OK){
    	strcpy(msg, "Mem read issue\r\n");
    	Send_UART_Message(msg);
    	}
    return data;
}

void IMU_Init(void) {
    char msg[30];
    uint8_t who_am_i=0;
    if (HAL_I2C_IsDeviceReady(&hi2c1, IMU_I2C_ADDR, 2, HAL_MAX_DELAY)!=HAL_OK){
    	strcpy(msg, "Device not ready\r\n");
    	Send_UART_Message(msg);
    }
    else{
    	strcpy(msg, "Device ready\r\n");
    	Send_UART_Message(msg);
    }

    who_am_i = IMU_ReadRegister(WHO_AM_I_REG);
    if (who_am_i == 0x71) {
        strcpy(msg, "IMU detected\r\n");
    } else {
        strcpy(msg, "Error: IMU not detected!\r\n");
    }

    Send_UART_Message(msg);
}

int IMU_ReadRegisters(uint16_t reg, uint8_t *data, uint16_t length) {
    if (HAL_I2C_Mem_Read(&hi2c1, IMU_I2C_ADDR, reg, 1, data, length, HAL_MAX_DELAY) != HAL_OK) {
        Send_UART_Message("I2C Read Error!\r\n");
        return 1;  // Return error
    }
    return 0;  // Return success
}

//void IMU_Enable_Gyroscope(void) {
//    uint8_t data;
//
//    // Read the current power management register (0x6B)
//    HAL_I2C_Mem_Read(&hi2c1, IMU_I2C_ADDR, 0x6B, 1, &data, 1, HAL_MAX_DELAY);
//
//    // Clear the sleep bit (bit 6) and set clock source to PLL with X-axis gyro (bits 2:0 = 001)
//    data &= ~(1 << 6);
//    data |= 0x01;  // Select PLL with X-axis gyro as clock source
//
//    // Write back to the register
//    HAL_I2C_Mem_Write(&hi2c1, IMU_I2C_ADDR, 0x6B, 1, &data, 1, HAL_MAX_DELAY);
//
//    Send_UART_Message("Gyroscope Enabled!\r\n");
//}

//READING ACCELERATION

int IMU_Check_Registers(uint16_t reg, uint8_t *data, uint16_t length) {
    if (HAL_I2C_Mem_Read(&hi2c1, IMU_I2C_ADDR, reg, 1, data, length, HAL_MAX_DELAY) != HAL_OK) {
        Send_UART_Message("I2C Read Error!\r\n");
        return 1;  // Return error
    }
    return 0;  // Return success
}

// BASIC OPERATION (READING RAW VALUES)

void IMU_ReadPrint_AccelGyro_NoOffset(int16_t *gyro_offset, int16_t *accel_offset) {
    uint8_t buffer[14];
    char msg[50];

    // Read accelerometer + gyroscope data
    if (IMU_ReadRegisters(0x3B, buffer, 14) != 0) {
        Send_UART_Message("Failed to read AccelGyro!\r\n");
        return;
    }

    int16_t accelX = ((buffer[0] << 8) | buffer[1]);
    int16_t accelY = ((buffer[2] << 8) | buffer[3]);
    int16_t accelZ = ((buffer[4] << 8) | buffer[5]);

    int16_t gyroX = ((buffer[8] << 8) | buffer[9]);
    int16_t gyroY = ((buffer[10] << 8) | buffer[11]);
    int16_t gyroZ = ((buffer[12] << 8) | buffer[13]);

    // Send messages via UART
    sprintf(msg, "Accel: %d %d %d\r\n", accelX, accelY, accelZ);
    Send_UART_Message(msg);
    sprintf(msg, "Gyro: %d %d %d\r\n", gyroX, gyroY, gyroZ);
    Send_UART_Message(msg);
}

//CHECKING REGISTERS OFFSET

void IMU_ReadGyroOffsetsReg(void) {
    uint8_t buffer[6];
    char msg[60];

    // Read gyroscope offset registers (0x13 to 0x18)
    if (IMU_ReadRegisters(0x13, buffer, 6) != 0) {
        Send_UART_Message("Failed to read Gyro offsets!\r\n");
        return;
    }

    int16_t gyroOffsetX = (buffer[0] << 8) | buffer[1];
    int16_t gyroOffsetY = (buffer[2] << 8) | buffer[3];
    int16_t gyroOffsetZ = (buffer[4] << 8) | buffer[5];

    // Print the offsets
    sprintf(msg, "Gyro Offsets registers: X=%d, Y=%d, Z=%d\r\n", gyroOffsetX, gyroOffsetY, gyroOffsetZ);
    Send_UART_Message(msg);
}

void IMU_ReadAccelOffsets(void) {
    uint8_t buffer[6];
    char msg[80];

    // Read accelerometer offset registers: 0x77–0x78 (X), 0x7A–0x7B (Y), 0x7D–0x7E (Z)
    if (IMU_ReadRegisters(0x77, buffer, 2) != 0 ||
        IMU_ReadRegisters(0x7A, buffer + 2, 2) != 0 ||
        IMU_ReadRegisters(0x7D, buffer + 4, 2) != 0) {
        Send_UART_Message("Failed to read Accel offsets!\r\n");
        return;
    }

    // Reconstruct 15-bit signed values
    int16_t accelOffsetX = ((int16_t)buffer[0] << 7) | (buffer[1] >> 1);
    int16_t accelOffsetY = ((int16_t)buffer[2] << 7) | (buffer[3] >> 1);
    int16_t accelOffsetZ = ((int16_t)buffer[4] << 7) | (buffer[5] >> 1);

    // Sign-extend to 16 bits if needed (15-bit value to signed int)
    if (accelOffsetX & 0x4000) accelOffsetX |= 0x8000;
    if (accelOffsetY & 0x4000) accelOffsetY |= 0x8000;
    if (accelOffsetZ & 0x4000) accelOffsetZ |= 0x8000;

    // Print the offsets
    sprintf(msg, "Accel Offsets registers: X=%d, Y=%d, Z=%d\r\n",
            accelOffsetX, accelOffsetY, accelOffsetZ);
    Send_UART_Message(msg);
}

//CHANGING OFFSET VALUES (CALIBRATION)

//void IMU_ApplyGyroCalibration(int16_t *gyro_offset) {
//    uint8_t data[6];
//
//    // Convert int16_t values to two bytes each
//    data[0] = (gyro_offset[0] >> 8) & 0xFF;
//    data[1] = gyro_offset[0] & 0xFF;
//    data[2] = (gyro_offset[1] >> 8) & 0xFF;
//    data[3] = gyro_offset[1] & 0xFF;
//    data[4] = (gyro_offset[2] >> 8) & 0xFF;
//    data[5] = gyro_offset[2] & 0xFF;
//
//    // Write offsets to gyroscope offset registers (XG_OFFSET_H/L, YG_OFFSET_H/L, ZG_OFFSET_H/L)
//    HAL_I2C_Mem_Write(&hi2c1, IMU_I2C_ADDR, 0x13, 1, data, 6, HAL_MAX_DELAY);
//
//    Send_UART_Message("Gyro offsets applied!\r\n");
//}
//
//void IMU_ApplyAccelCalibration(int16_t *accel_offset) {
//    // Os offsets devem ser ajustados para 15 bits, como requerido pelo MPU-9250
//    uint8_t data[6];
//
//    // X offset
//    data[0] = (accel_offset[0] >> 7) & 0xFF;         // High 8 bits (bits 14:7)
//    data[1] = (accel_offset[0] << 1) & 0xFE;         // Low 7 bits (bits 6:0), bit 0 = 0
//
//    // Y offset
//    data[2] = (accel_offset[1] >> 7) & 0xFF;
//    data[3] = (accel_offset[1] << 1) & 0xFE;
//
//    // Z offset
//    data[4] = (accel_offset[2] >> 7) & 0xFF;
//    data[5] = (accel_offset[2] << 1) & 0xFE;
//
//    // Escreve nos registradores 0x77 (XA_OFFSET_H) a 0x7E (ZA_OFFSET_L)
//    HAL_I2C_Mem_Write(&hi2c1, IMU_I2C_ADDR, 0x77, 1, data, 2, HAL_MAX_DELAY); // X
//    HAL_I2C_Mem_Write(&hi2c1, IMU_I2C_ADDR, 0x7A, 1, data + 2, 2, HAL_MAX_DELAY); // Y
//    HAL_I2C_Mem_Write(&hi2c1, IMU_I2C_ADDR, 0x7D, 1, data + 4, 2, HAL_MAX_DELAY); // Z
//
//    Send_UART_Message("Accel offsets applied to registers!\r\n");
//}

void IMU_Calibrate(int16_t *gyro_offset, int16_t *accel_offset) {
    uint8_t buffer[14];
    int32_t accel_sum[3] = {0, 0, 0};
    int32_t gyro_sum[3] = {0, 0, 0};
    int num_samples = 1000;  // Number of samples for averaging

    Send_UART_Message("Starting IMU Calibration...\r\n");

    for (int i = 0; i < num_samples; i++) {
        if (IMU_ReadRegisters(0x3B, buffer, 14) != 0) {
            Send_UART_Message("Failed to read IMU data!\r\n");
            return;
        }

        int16_t accelX = (buffer[0] << 8) | buffer[1];
        int16_t accelY = (buffer[2] << 8) | buffer[3];
        int16_t accelZ = (buffer[4] << 8) | buffer[5];

        int16_t gyroX = (buffer[8] << 8) | buffer[9];
        int16_t gyroY = (buffer[10] << 8) | buffer[11];
        int16_t gyroZ = (buffer[12] << 8) | buffer[13];

        // Sum values for averaging
        accel_sum[0] += accelX;
        accel_sum[1] += accelY;
        accel_sum[2] += (accelZ - 16384);  // Remove gravity effect

        gyro_sum[0] += gyroX;
        gyro_sum[1] += gyroY;
        gyro_sum[2] += gyroZ;

        Delay_ms(2);
    }

    // Compute average offset
    for (int i = 0; i < 3; i++) {
        accel_offset[i] = accel_sum[i] / num_samples;
        gyro_offset[i] = gyro_sum[i] / num_samples;
    }

    // Print calculated offsets for debugging
//    char msg[50];
//    sprintf(msg, "Accel Offsets: %d, %d, %d\r\n", accel_offset[0], accel_offset[1], accel_offset[2]);
//    Send_UART_Message(msg);
//    sprintf(msg, "Gyro Offsets: %d, %d, %d\r\n", gyro_offset[0], gyro_offset[1], gyro_offset[2]);
//    Send_UART_Message(msg);

    // Apply offsets to registers (DO NOT USE)
    //IMU_ApplyGyroCalibration(gyro_offset);
    //IMU_ApplyAccelCalibration(accel_offset);
}

//IT CONVERTS THE DATA INTO READABLE VALUES

//void Convert_IMU_Data(int16_t *raw_accel, int16_t *raw_gyro, float *accel_g, float *gyro_dps) {
//    // Define sensitivity values (Change these based on your configuration)
//    float accel_sensitivity = 16384.0;  // ±2g => 16384 LSB/g
//    float gyro_sensitivity = 131.0;     // ±250°/s => 131 LSB/°/s
//
//    // Convert accelerometer values to g
//    accel_g[0] = raw_accel[0] / accel_sensitivity;
//    accel_g[1] = raw_accel[1] / accel_sensitivity;
//    accel_g[2] = raw_accel[2] / accel_sensitivity;
//
//    // Convert gyroscope values to °/s
//    gyro_dps[0] = raw_gyro[0] / gyro_sensitivity;
//    gyro_dps[1] = raw_gyro[1] / gyro_sensitivity;
//    gyro_dps[2] = raw_gyro[2] / gyro_sensitivity;
//}

void Convert_IMU_Data_All(int16_t *raw_accel, int16_t *raw_gyro, float *accel_g, float *gyro_dps, float *gyro_angle, float dt) {
    float accel_sensitivity = 16384.0f;  // ±2g
    float gyro_sensitivity  = 131.0f;    // ±250°/s

    // Static variable to hold cumulative integration between calls
    static float integrated_angle[3] = {0.0f, 0.0f, 0.0f};

    // Convert accelerometer values to g
    // Convert gyroscope values to °/s and integrate to get angle
    for (int i = 0; i < 3; i++) {
        accel_g[i] = raw_accel[i] / accel_sensitivity;

        gyro_dps[i] = raw_gyro[i] / gyro_sensitivity;

        integrated_angle[i] += gyro_dps[i] * dt;
        gyro_angle[i] = integrated_angle[i];
    }
}


//void IMU_ReadPrint_AccelGyro_Converted(int16_t *gyro_offset, int16_t *accel_offset) {
//    uint8_t buffer[14];
//    char msg[50];
//    char num_str[10];
//
//    // Read raw accelerometer and gyroscope data
//    if (IMU_ReadRegisters(0x3B, buffer, 14) != 0) {
//        Send_UART_Message("Failed to read IMU data!\r\n");
//        return;
//    }
//
//    int16_t raw_accel[3];
//    int16_t raw_gyro[3];
//    float accel_g[3];
//    float gyro_dps[3];
//
//    // Apply offsets and store raw values
//    raw_accel[0] = ((buffer[0] << 8) | buffer[1]) - accel_offset[0];
//    raw_accel[1] = ((buffer[2] << 8) | buffer[3]) - accel_offset[1];
//    raw_accel[2] = ((buffer[4] << 8) | buffer[5]) - accel_offset[2];
//
//    raw_gyro[0] = ((buffer[8] << 8) | buffer[9]) - gyro_offset[0];
//    raw_gyro[1] = ((buffer[10] << 8) | buffer[11]) - gyro_offset[1];
//    raw_gyro[2] = ((buffer[12] << 8) | buffer[13]) - gyro_offset[2];
//
//    // Convert to conventional units
//    Convert_IMU_Data(raw_accel, raw_gyro, accel_g, gyro_dps);
//
//    // Print accelerometer values (g)
//    strcpy(msg, "Accel (g): X=");
//    Send_UART_Message(msg);
//    itoa((int)(accel_g[0] * 100), num_str, 10);
//    Send_UART_Message(num_str);
//    Send_UART_Message(" Y=");
//    itoa((int)(accel_g[1] * 100), num_str, 10);
//    Send_UART_Message(num_str);
//    Send_UART_Message(" Z=");
//    itoa((int)(accel_g[2] * 100), num_str, 10);
//    Send_UART_Message(num_str);
//    Send_UART_Message("\r\n");
//
//    // Print gyroscope values (°/s)
//    strcpy(msg, "Gyro (degree/s): X=");
//    Send_UART_Message(msg);
//    itoa((int)(gyro_dps[0] * 100), num_str, 10);
//    Send_UART_Message(num_str);
//    Send_UART_Message(" Y=");
//    itoa((int)(gyro_dps[1] * 100), num_str, 10);
//    Send_UART_Message(num_str);
//    Send_UART_Message(" Z=");
//    itoa((int)(gyro_dps[2] * 100), num_str, 10);
//    Send_UART_Message(num_str);
//    Send_UART_Message("\r\n");
//}

void IMU_ReadAccelGyro_Converted_All(int16_t *gyro_offset, int16_t *accel_offset, float dt,
                                     float *accel_g, float *gyro_dps, float *gyro_angle) {
    uint8_t buffer[14];

    if (IMU_ReadRegisters(0x3B, buffer, 14) != 0) {
        Send_UART_Message("Failed to read IMU data!\r\n");
        return;
    }

    int16_t raw_accel[3];
    int16_t raw_gyro[3];

    // Apply offsets
    raw_accel[0] = ((buffer[0] << 8) | buffer[1]) - accel_offset[0];
    raw_accel[1] = ((buffer[2] << 8) | buffer[3]) - accel_offset[1];
    raw_accel[2] = ((buffer[4] << 8) | buffer[5]) - accel_offset[2];

    raw_gyro[0] = ((buffer[8] << 8) | buffer[9]) - gyro_offset[0];
    raw_gyro[1] = ((buffer[10] << 8) | buffer[11]) - gyro_offset[1];
    raw_gyro[2] = ((buffer[12] << 8) | buffer[13]) - gyro_offset[2];

    // Convert to g and dps, and integrate gyroscope values into angles
    Convert_IMU_Data_All(raw_accel, raw_gyro, accel_g, gyro_dps, gyro_angle, dt);
}

// PRINT ALL VECTORS FROM IMU_ReadAccelGyro_Converted_All

void IMU_Print_AccelGyro_Vectors(float *accel_g, float *gyro_dps, float *gyro_angle) {
    char num_str[10];

    // -------- Accelerometer Output --------
    Send_UART_Message("Accel (g): X=");
    itoa((int)(accel_g[0] * 100), num_str, 10);
    Send_UART_Message(num_str);
    Send_UART_Message(" Y=");
    itoa((int)(accel_g[1] * 100), num_str, 10);
    Send_UART_Message(num_str);
    Send_UART_Message(" Z=");
    itoa((int)(accel_g[2] * 100), num_str, 10);
    Send_UART_Message(num_str);
    Send_UART_Message("\r\n");

    // -------- Gyroscope Output (°/s) --------
    Send_UART_Message("Gyro (dps): X=");
    itoa((int)(gyro_dps[0] * 100), num_str, 10);
    Send_UART_Message(num_str);
    Send_UART_Message(" Y=");
    itoa((int)(gyro_dps[1] * 100), num_str, 10);
    Send_UART_Message(num_str);
    Send_UART_Message(" Z=");
    itoa((int)(gyro_dps[2] * 100), num_str, 10);
    Send_UART_Message(num_str);
    Send_UART_Message("\r\n");

    // -------- Integrated Gyro Angles --------
    Send_UART_Message("Gyro Angles (degrees): X=");
    itoa((int)(gyro_angle[0]), num_str, 10);
    Send_UART_Message(num_str);
    Send_UART_Message(" Y=");
    itoa((int)(gyro_angle[1]), num_str, 10);
    Send_UART_Message(num_str);
    Send_UART_Message(" Z=");
    itoa((int)(gyro_angle[2]), num_str, 10);
    Send_UART_Message(num_str);
    Send_UART_Message("\r\n");
}

//FORCE SENSOR SENSITIVITY

void IMU_ForceSensorSensitivity(void) {
    uint8_t accel_config = 0x00; // ±2g
    uint8_t gyro_config  = 0x00; // ±250°/s

    HAL_I2C_Mem_Write(&hi2c1, IMU_I2C_ADDR, 0x1C, 1, &accel_config, 1, HAL_MAX_DELAY);
    HAL_I2C_Mem_Write(&hi2c1, IMU_I2C_ADDR, 0x1B, 1, &gyro_config, 1, HAL_MAX_DELAY);

    Send_UART_Message("Sensor sensitivities set: Accel=±2g, Gyro=±250°/s\r\n");
}

//TO ASSURE THE SENSOR IS ENABLED

void IMU_Enable_Sensors(void) {
    uint8_t data = 0x00;  // Enable all sensors (Gyro + Accel)

    // Write to Power Management 2 Register (0x6C)
    HAL_I2C_Mem_Write(&hi2c1, IMU_I2C_ADDR, 0x6C, 1, &data, 1, HAL_MAX_DELAY);

    Send_UART_Message("All Sensors Enabled!\r\n");
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
    int16_t gyro_offset[3] = {0, 0, 0};
    int16_t accel_offset[3] = {0, 0, 0};
    uint32_t prev_time, curr_time;
    float dt;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  IMU_Init();
  IMU_Calibrate(gyro_offset, accel_offset);

  prev_time = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	    float accel_g[3], gyro_dps[3], gyro_angle[3];
	    curr_time = HAL_GetTick();
	    dt = (curr_time - prev_time) / 1000.0;
	    if (dt <= 0) dt = 0.01;
	    prev_time = curr_time;

//	    IMU_ReadAccelGyro_Converted_deltaT(gyro_offset, accel_offset, dt);
	    IMU_ReadAccelGyro_Converted_All(gyro_offset, accel_offset, dt, accel_g, gyro_dps, gyro_angle);
	    IMU_Print_AccelGyro_Vectors(accel_g, gyro_dps, gyro_angle);
	    Delay_ms(10);  // lower possible = 10ms

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

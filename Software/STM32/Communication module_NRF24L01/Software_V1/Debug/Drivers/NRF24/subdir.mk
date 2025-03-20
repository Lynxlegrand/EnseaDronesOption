################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/NRF24/NRF24.c 

OBJS += \
./Drivers/NRF24/NRF24.o 

C_DEPS += \
./Drivers/NRF24/NRF24.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/NRF24/%.o Drivers/NRF24/%.su Drivers/NRF24/%.cyclo: ../Drivers/NRF24/%.c Drivers/NRF24/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I"C:/Users/hugof/Desktop/GITHUB/EnseaDronesOption/Software/STM32/Communication module_NRF24L01/Software/Drivers/NRF24" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-NRF24

clean-Drivers-2f-NRF24:
	-$(RM) ./Drivers/NRF24/NRF24.cyclo ./Drivers/NRF24/NRF24.d ./Drivers/NRF24/NRF24.o ./Drivers/NRF24/NRF24.su

.PHONY: clean-Drivers-2f-NRF24


################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Peripherals/FRAM.c \
../Core/Src/Peripherals/INA230.c \
../Core/Src/Peripherals/TMP468.c 

OBJS += \
./Core/Src/Peripherals/FRAM.o \
./Core/Src/Peripherals/INA230.o \
./Core/Src/Peripherals/TMP468.o 

C_DEPS += \
./Core/Src/Peripherals/FRAM.d \
./Core/Src/Peripherals/INA230.d \
./Core/Src/Peripherals/TMP468.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Peripherals/%.o Core/Src/Peripherals/%.su Core/Src/Peripherals/%.cyclo: ../Core/Src/Peripherals/%.c Core/Src/Peripherals/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U073xx -c -I../Core/Inc -I../Drivers/STM32U0xx_HAL_Driver/Inc -I../Drivers/STM32U0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U0xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0/ -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Peripherals

clean-Core-2f-Src-2f-Peripherals:
	-$(RM) ./Core/Src/Peripherals/FRAM.cyclo ./Core/Src/Peripherals/FRAM.d ./Core/Src/Peripherals/FRAM.o ./Core/Src/Peripherals/FRAM.su ./Core/Src/Peripherals/INA230.cyclo ./Core/Src/Peripherals/INA230.d ./Core/Src/Peripherals/INA230.o ./Core/Src/Peripherals/INA230.su ./Core/Src/Peripherals/TMP468.cyclo ./Core/Src/Peripherals/TMP468.d ./Core/Src/Peripherals/TMP468.o ./Core/Src/Peripherals/TMP468.su

.PHONY: clean-Core-2f-Src-2f-Peripherals


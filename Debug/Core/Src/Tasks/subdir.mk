################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Tasks/banner.c \
../Core/Src/Tasks/ledManager.c \
../Core/Src/Tasks/logPower.c \
../Core/Src/Tasks/logTemp.c \
../Core/Src/Tasks/powerAlertService.c \
../Core/Src/Tasks/powerUp.c \
../Core/Src/Tasks/printer.c \
../Core/Src/Tasks/serviceWatchdog.c 

OBJS += \
./Core/Src/Tasks/banner.o \
./Core/Src/Tasks/ledManager.o \
./Core/Src/Tasks/logPower.o \
./Core/Src/Tasks/logTemp.o \
./Core/Src/Tasks/powerAlertService.o \
./Core/Src/Tasks/powerUp.o \
./Core/Src/Tasks/printer.o \
./Core/Src/Tasks/serviceWatchdog.o 

C_DEPS += \
./Core/Src/Tasks/banner.d \
./Core/Src/Tasks/ledManager.d \
./Core/Src/Tasks/logPower.d \
./Core/Src/Tasks/logTemp.d \
./Core/Src/Tasks/powerAlertService.d \
./Core/Src/Tasks/powerUp.d \
./Core/Src/Tasks/printer.d \
./Core/Src/Tasks/serviceWatchdog.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Tasks/%.o Core/Src/Tasks/%.su Core/Src/Tasks/%.cyclo: ../Core/Src/Tasks/%.c Core/Src/Tasks/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U073xx -c -I../Core/Inc -I../Drivers/STM32U0xx_HAL_Driver/Inc -I../Drivers/STM32U0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U0xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0/ -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Tasks

clean-Core-2f-Src-2f-Tasks:
	-$(RM) ./Core/Src/Tasks/banner.cyclo ./Core/Src/Tasks/banner.d ./Core/Src/Tasks/banner.o ./Core/Src/Tasks/banner.su ./Core/Src/Tasks/ledManager.cyclo ./Core/Src/Tasks/ledManager.d ./Core/Src/Tasks/ledManager.o ./Core/Src/Tasks/ledManager.su ./Core/Src/Tasks/logPower.cyclo ./Core/Src/Tasks/logPower.d ./Core/Src/Tasks/logPower.o ./Core/Src/Tasks/logPower.su ./Core/Src/Tasks/logTemp.cyclo ./Core/Src/Tasks/logTemp.d ./Core/Src/Tasks/logTemp.o ./Core/Src/Tasks/logTemp.su ./Core/Src/Tasks/powerAlertService.cyclo ./Core/Src/Tasks/powerAlertService.d ./Core/Src/Tasks/powerAlertService.o ./Core/Src/Tasks/powerAlertService.su ./Core/Src/Tasks/powerUp.cyclo ./Core/Src/Tasks/powerUp.d ./Core/Src/Tasks/powerUp.o ./Core/Src/Tasks/powerUp.su ./Core/Src/Tasks/printer.cyclo ./Core/Src/Tasks/printer.d ./Core/Src/Tasks/printer.o ./Core/Src/Tasks/printer.su ./Core/Src/Tasks/serviceWatchdog.cyclo ./Core/Src/Tasks/serviceWatchdog.d ./Core/Src/Tasks/serviceWatchdog.o ./Core/Src/Tasks/serviceWatchdog.su

.PHONY: clean-Core-2f-Src-2f-Tasks


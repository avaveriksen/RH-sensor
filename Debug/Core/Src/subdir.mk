################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/HYT271_driver.c \
../Core/Src/PCA9548A.c \
../Core/Src/SHT45_driver.c \
../Core/Src/i2c_driver.c \
../Core/Src/main.c \
../Core/Src/serial_driver.c \
../Core/Src/stm32f3xx_hal_msp.c \
../Core/Src/stm32f3xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f3xx.c 

OBJS += \
./Core/Src/HYT271_driver.o \
./Core/Src/PCA9548A.o \
./Core/Src/SHT45_driver.o \
./Core/Src/i2c_driver.o \
./Core/Src/main.o \
./Core/Src/serial_driver.o \
./Core/Src/stm32f3xx_hal_msp.o \
./Core/Src/stm32f3xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f3xx.o 

C_DEPS += \
./Core/Src/HYT271_driver.d \
./Core/Src/PCA9548A.d \
./Core/Src/SHT45_driver.d \
./Core/Src/i2c_driver.d \
./Core/Src/main.d \
./Core/Src/serial_driver.d \
./Core/Src/stm32f3xx_hal_msp.d \
./Core/Src/stm32f3xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f3xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/HYT271_driver.cyclo ./Core/Src/HYT271_driver.d ./Core/Src/HYT271_driver.o ./Core/Src/HYT271_driver.su ./Core/Src/PCA9548A.cyclo ./Core/Src/PCA9548A.d ./Core/Src/PCA9548A.o ./Core/Src/PCA9548A.su ./Core/Src/SHT45_driver.cyclo ./Core/Src/SHT45_driver.d ./Core/Src/SHT45_driver.o ./Core/Src/SHT45_driver.su ./Core/Src/i2c_driver.cyclo ./Core/Src/i2c_driver.d ./Core/Src/i2c_driver.o ./Core/Src/i2c_driver.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/serial_driver.cyclo ./Core/Src/serial_driver.d ./Core/Src/serial_driver.o ./Core/Src/serial_driver.su ./Core/Src/stm32f3xx_hal_msp.cyclo ./Core/Src/stm32f3xx_hal_msp.d ./Core/Src/stm32f3xx_hal_msp.o ./Core/Src/stm32f3xx_hal_msp.su ./Core/Src/stm32f3xx_it.cyclo ./Core/Src/stm32f3xx_it.d ./Core/Src/stm32f3xx_it.o ./Core/Src/stm32f3xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f3xx.cyclo ./Core/Src/system_stm32f3xx.d ./Core/Src/system_stm32f3xx.o ./Core/Src/system_stm32f3xx.su

.PHONY: clean-Core-2f-Src


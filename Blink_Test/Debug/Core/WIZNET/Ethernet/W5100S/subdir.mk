################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/WIZNET/Ethernet/W5100S/w5100s.c 

OBJS += \
./Core/WIZNET/Ethernet/W5100S/w5100s.o 

C_DEPS += \
./Core/WIZNET/Ethernet/W5100S/w5100s.d 


# Each subdirectory must supply rules for building sources it contributes
Core/WIZNET/Ethernet/W5100S/%.o Core/WIZNET/Ethernet/W5100S/%.su Core/WIZNET/Ethernet/W5100S/%.cyclo: ../Core/WIZNET/Ethernet/W5100S/%.c Core/WIZNET/Ethernet/W5100S/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I"C:/Users/maxti/dev/STM32/Blink_Test/Core/App" -I"C:/Users/maxti/dev/STM32/Blink_Test/Core/WIZNET/Internet/DHCP" -I../Core/Inc -I../Core/WIZNET/Ethernet -I../Core/WIZNET/Ethernet/W5500 -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-WIZNET-2f-Ethernet-2f-W5100S

clean-Core-2f-WIZNET-2f-Ethernet-2f-W5100S:
	-$(RM) ./Core/WIZNET/Ethernet/W5100S/w5100s.cyclo ./Core/WIZNET/Ethernet/W5100S/w5100s.d ./Core/WIZNET/Ethernet/W5100S/w5100s.o ./Core/WIZNET/Ethernet/W5100S/w5100s.su

.PHONY: clean-Core-2f-WIZNET-2f-Ethernet-2f-W5100S


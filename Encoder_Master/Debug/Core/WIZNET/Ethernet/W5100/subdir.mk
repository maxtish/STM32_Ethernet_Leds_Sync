################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/WIZNET/Ethernet/W5100/w5100.c 

OBJS += \
./Core/WIZNET/Ethernet/W5100/w5100.o 

C_DEPS += \
./Core/WIZNET/Ethernet/W5100/w5100.d 


# Each subdirectory must supply rules for building sources it contributes
Core/WIZNET/Ethernet/W5100/%.o Core/WIZNET/Ethernet/W5100/%.su Core/WIZNET/Ethernet/W5100/%.cyclo: ../Core/WIZNET/Ethernet/W5100/%.c Core/WIZNET/Ethernet/W5100/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../USB_DEVICE/App -I"C:/Users/maxti/dev/STM32/Encoder_Master/Core/App" -I"C:/Users/maxti/dev/STM32/Encoder_Master/Core/WIZNET" -I"C:/Users/maxti/dev/STM32/Encoder_Master/Core/WIZNET/Ethernet" -I"C:/Users/maxti/dev/STM32/Encoder_Master/Core/WIZNET/Ethernet/W5500" -I"C:/Users/maxti/dev/STM32/Encoder_Master/Core/WIZNET/Internet" -I"C:/Users/maxti/dev/STM32/Encoder_Master/Core/WIZNET/Internet/DHCP" -I../USB_DEVICE/Target -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-WIZNET-2f-Ethernet-2f-W5100

clean-Core-2f-WIZNET-2f-Ethernet-2f-W5100:
	-$(RM) ./Core/WIZNET/Ethernet/W5100/w5100.cyclo ./Core/WIZNET/Ethernet/W5100/w5100.d ./Core/WIZNET/Ethernet/W5100/w5100.o ./Core/WIZNET/Ethernet/W5100/w5100.su

.PHONY: clean-Core-2f-WIZNET-2f-Ethernet-2f-W5100


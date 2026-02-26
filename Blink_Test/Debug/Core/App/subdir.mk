################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/App/app_ethernet.c \
../Core/App/app_led.c \
../Core/App/app_usb_log.c \
../Core/App/ws2812b.c 

OBJS += \
./Core/App/app_ethernet.o \
./Core/App/app_led.o \
./Core/App/app_usb_log.o \
./Core/App/ws2812b.o 

C_DEPS += \
./Core/App/app_ethernet.d \
./Core/App/app_led.d \
./Core/App/app_usb_log.d \
./Core/App/ws2812b.d 


# Each subdirectory must supply rules for building sources it contributes
Core/App/%.o Core/App/%.su Core/App/%.cyclo: ../Core/App/%.c Core/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I"C:/Users/maxti/dev/STM32/Blink_Test/Core/App" -I"C:/Users/maxti/dev/STM32/Blink_Test/Core/WIZNET/Internet/DHCP" -I../Core/Inc -I../Core/WIZNET/Ethernet -I../Core/WIZNET/Ethernet/W5500 -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-App

clean-Core-2f-App:
	-$(RM) ./Core/App/app_ethernet.cyclo ./Core/App/app_ethernet.d ./Core/App/app_ethernet.o ./Core/App/app_ethernet.su ./Core/App/app_led.cyclo ./Core/App/app_led.d ./Core/App/app_led.o ./Core/App/app_led.su ./Core/App/app_usb_log.cyclo ./Core/App/app_usb_log.d ./Core/App/app_usb_log.o ./Core/App/app_usb_log.su ./Core/App/ws2812b.cyclo ./Core/App/ws2812b.d ./Core/App/ws2812b.o ./Core/App/ws2812b.su

.PHONY: clean-Core-2f-App


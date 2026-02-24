################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/App/app_led_blinking.c \
../Core/App/app_uart_logic.c \
../Core/App/app_usb_log.c 

OBJS += \
./Core/App/app_led_blinking.o \
./Core/App/app_uart_logic.o \
./Core/App/app_usb_log.o 

C_DEPS += \
./Core/App/app_led_blinking.d \
./Core/App/app_uart_logic.d \
./Core/App/app_usb_log.d 


# Each subdirectory must supply rules for building sources it contributes
Core/App/%.o Core/App/%.su Core/App/%.cyclo: ../Core/App/%.c Core/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I"C:/Users/maxti/dev/STM32/encoder/Core/App" -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-App

clean-Core-2f-App:
	-$(RM) ./Core/App/app_led_blinking.cyclo ./Core/App/app_led_blinking.d ./Core/App/app_led_blinking.o ./Core/App/app_led_blinking.su ./Core/App/app_uart_logic.cyclo ./Core/App/app_uart_logic.d ./Core/App/app_uart_logic.o ./Core/App/app_uart_logic.su ./Core/App/app_usb_log.cyclo ./Core/App/app_usb_log.d ./Core/App/app_usb_log.o ./Core/App/app_usb_log.su

.PHONY: clean-Core-2f-App


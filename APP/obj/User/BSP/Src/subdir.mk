################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/BSP/Src/ADC_k.c \
../User/BSP/Src/DAC_k.c \
../User/BSP/Src/Encoder_k.c \
../User/BSP/Src/Key_k.c \
../User/BSP/Src/LED_k.c \
../User/BSP/Src/SerialTFT_K.c \
../User/BSP/Src/Timer_k.c \
../User/BSP/Src/WS2812_k.c 

C_DEPS += \
./User/BSP/Src/ADC_k.d \
./User/BSP/Src/DAC_k.d \
./User/BSP/Src/Encoder_k.d \
./User/BSP/Src/Key_k.d \
./User/BSP/Src/LED_k.d \
./User/BSP/Src/SerialTFT_K.d \
./User/BSP/Src/Timer_k.d \
./User/BSP/Src/WS2812_k.d 

OBJS += \
./User/BSP/Src/ADC_k.o \
./User/BSP/Src/DAC_k.o \
./User/BSP/Src/Encoder_k.o \
./User/BSP/Src/Key_k.o \
./User/BSP/Src/LED_k.o \
./User/BSP/Src/SerialTFT_K.o \
./User/BSP/Src/Timer_k.o \
./User/BSP/Src/WS2812_k.o 


EXPANDS += \
./User/BSP/Src/ADC_k.c.253r.expand \
./User/BSP/Src/DAC_k.c.253r.expand \
./User/BSP/Src/Encoder_k.c.253r.expand \
./User/BSP/Src/Key_k.c.253r.expand \
./User/BSP/Src/LED_k.c.253r.expand \
./User/BSP/Src/SerialTFT_K.c.253r.expand \
./User/BSP/Src/Timer_k.c.253r.expand \
./User/BSP/Src/WS2812_k.c.253r.expand 



# Each subdirectory must supply rules for building sources it contributes
User/BSP/Src/%.o: ../User/BSP/Src/%.c
	@	riscv-wch-elf-gcc -march=rv32ec_zmmul_xw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -fmax-errors=20 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"c:/Users/Kun/Desktop/数控电源-啄木鸟/Debug" -I"c:/Users/Kun/Desktop/数控电源-啄木鸟/Core" -I"c:/Users/Kun/Desktop/数控电源-啄木鸟/User" -I"c:/Users/Kun/Desktop/数控电源-啄木鸟/Peripheral/inc" -I"c:/Users/Kun/Desktop/数控电源-啄木鸟/User/BSP/Inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"


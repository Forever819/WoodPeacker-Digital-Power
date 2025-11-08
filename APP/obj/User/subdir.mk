################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32v00X_it.c \
../User/main.c \
../User/system_ch32v00X.c 

C_DEPS += \
./User/ch32v00X_it.d \
./User/main.d \
./User/system_ch32v00X.d 

OBJS += \
./User/ch32v00X_it.o \
./User/main.o \
./User/system_ch32v00X.o 


EXPANDS += \
./User/ch32v00X_it.c.253r.expand \
./User/main.c.253r.expand \
./User/system_ch32v00X.c.253r.expand 



# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	riscv-wch-elf-gcc -march=rv32ec_zmmul_xw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -fmax-errors=20 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"c:/Users/Kun/Desktop/数控电源-啄木鸟/Debug" -I"c:/Users/Kun/Desktop/数控电源-啄木鸟/Core" -I"c:/Users/Kun/Desktop/数控电源-啄木鸟/User" -I"c:/Users/Kun/Desktop/数控电源-啄木鸟/Peripheral/inc" -I"c:/Users/Kun/Desktop/数控电源-啄木鸟/User/BSP/Inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"


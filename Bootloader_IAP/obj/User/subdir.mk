################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/IAP.c \
../User/SerialTFT_K.c \
../User/ch32v00X_it.c \
../User/main.c \
../User/system_ch32v00X.c 

C_DEPS += \
./User/IAP.d \
./User/SerialTFT_K.d \
./User/ch32v00X_it.d \
./User/main.d \
./User/system_ch32v00X.d 

OBJS += \
./User/IAP.o \
./User/SerialTFT_K.o \
./User/ch32v00X_it.o \
./User/main.o \
./User/system_ch32v00X.o 


EXPANDS += \
./User/IAP.c.253r.expand \
./User/SerialTFT_K.c.253r.expand \
./User/ch32v00X_it.c.253r.expand \
./User/main.c.253r.expand \
./User/system_ch32v00X.c.253r.expand 



# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	riscv-wch-elf-gcc -march=rv32ec_zmmul_xw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"c:/Users/Kun/Desktop/CH32V006F8P6-iap/Debug" -I"c:/Users/Kun/Desktop/CH32V006F8P6-iap/Core" -I"c:/Users/Kun/Desktop/CH32V006F8P6-iap/User" -I"c:/Users/Kun/Desktop/CH32V006F8P6-iap/Peripheral/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"


################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/EPPROM.c \
../src/erase.c \
../src/main.c \
../src/status_callback.c 

OBJS += \
./src/EPPROM.o \
./src/erase.o \
./src/main.o \
./src/status_callback.o 

C_DEPS += \
./src/EPPROM.d \
./src/erase.d \
./src/main.d \
./src/status_callback.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/EPPROM.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



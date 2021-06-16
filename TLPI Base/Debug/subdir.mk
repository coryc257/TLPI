################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../error_functions.c \
../exec_container.c \
../get_num.c \
../main.c 

OBJS += \
./error_functions.o \
./exec_container.o \
./get_num.o \
./main.o 

C_DEPS += \
./error_functions.d \
./exec_container.d \
./get_num.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I"/home/cory/eclipse-workspace/TLPI Base/lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



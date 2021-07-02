################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../curr_time.c \
../daemonize.c \
../error_functions.c \
../exec_container.c \
../get_num.c \
../main.c \
../ugid_functions.c 

OBJS += \
./curr_time.o \
./daemonize.o \
./error_functions.o \
./exec_container.o \
./get_num.o \
./main.o \
./ugid_functions.o 

C_DEPS += \
./curr_time.d \
./daemonize.d \
./error_functions.d \
./exec_container.d \
./get_num.d \
./main.d \
./ugid_functions.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I"/home/cory/eclipse-workspace/TLPI Base/lib" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



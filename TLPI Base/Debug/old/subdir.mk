################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../old/curr_time.c \
../old/daemonize.c \
../old/error_functions.c \
../old/exec_container.c \
../old/get_num.c \
../old/main.c \
../old/ugid_functions.c 

OBJS += \
./old/curr_time.o \
./old/daemonize.o \
./old/error_functions.o \
./old/exec_container.o \
./old/get_num.o \
./old/main.o \
./old/ugid_functions.o 

C_DEPS += \
./old/curr_time.d \
./old/daemonize.d \
./old/error_functions.d \
./old/exec_container.d \
./old/get_num.d \
./old/main.d \
./old/ugid_functions.d 


# Each subdirectory must supply rules for building sources it contributes
old/%.o: ../old/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I"/home/cory/eclipse-workspace/TLPI Base/lib" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



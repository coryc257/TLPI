################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../sysinfo/12_1.c \
../sysinfo/procfs_pidmax.c \
../sysinfo/t_uname.c 

OBJS += \
./sysinfo/12_1.o \
./sysinfo/procfs_pidmax.o \
./sysinfo/t_uname.o 

C_DEPS += \
./sysinfo/12_1.d \
./sysinfo/procfs_pidmax.d \
./sysinfo/t_uname.d 


# Each subdirectory must supply rules for building sources it contributes
sysinfo/%.o: ../sysinfo/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I"/home/cory/eclipse-workspace/TLPI Base/lib" -I"/home/cory/eclipse-workspace/8/lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



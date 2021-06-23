################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../proc/display_env.c \
../proc/i_tripped.c \
../proc/longjmp.c \
../proc/mem_segments.c \
../proc/modify_env.c \
../proc/necho.c \
../proc/setjmp_vars.c 

OBJS += \
./proc/display_env.o \
./proc/i_tripped.o \
./proc/longjmp.o \
./proc/mem_segments.o \
./proc/modify_env.o \
./proc/necho.o \
./proc/setjmp_vars.o 

C_DEPS += \
./proc/display_env.d \
./proc/i_tripped.d \
./proc/longjmp.d \
./proc/mem_segments.d \
./proc/modify_env.d \
./proc/necho.d \
./proc/setjmp_vars.d 


# Each subdirectory must supply rules for building sources it contributes
proc/%.o: ../proc/%.c proc/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I"/home/cory/tlpi_workspace/TLPI/TLPI Base/lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



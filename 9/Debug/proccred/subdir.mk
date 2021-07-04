################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../proccred/exercise_9_3.c \
../proccred/idshow.c 

OBJS += \
./proccred/exercise_9_3.o \
./proccred/idshow.o 

C_DEPS += \
./proccred/exercise_9_3.d \
./proccred/idshow.d 


# Each subdirectory must supply rules for building sources it contributes
proccred/%.o: ../proccred/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I"/home/cory/tlpi_workspace/TLPI/TLPI Base/lib" -I"/home/cory/tlpi_workspace/TLPI/7/lib" -I"/home/cory/tlpi_workspace/TLPI/8/lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



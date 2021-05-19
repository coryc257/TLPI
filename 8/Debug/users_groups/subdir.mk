################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../users_groups/check_password.c \
../users_groups/exercise_8_2.c \
../users_groups/ugid_functions.c 

OBJS += \
./users_groups/check_password.o \
./users_groups/exercise_8_2.o \
./users_groups/ugid_functions.o 

C_DEPS += \
./users_groups/check_password.d \
./users_groups/exercise_8_2.d \
./users_groups/ugid_functions.d 


# Each subdirectory must supply rules for building sources it contributes
users_groups/%.o: ../users_groups/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I"/home/cory/eclipse-workspace/TLPI Base/lib" -I"/home/cory/eclipse-workspace/7/lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



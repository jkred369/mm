################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../lib/common/test/ConditionTest.cpp \
../lib/common/test/TestRunner.cpp 

OBJS += \
./lib/common/test/ConditionTest.o \
./lib/common/test/TestRunner.o 

CPP_DEPS += \
./lib/common/test/ConditionTest.d \
./lib/common/test/TestRunner.d 


# Each subdirectory must supply rules for building sources it contributes
lib/common/test/%.o: ../lib/common/test/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



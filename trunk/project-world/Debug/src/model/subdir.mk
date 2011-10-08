################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/model/Creature.cpp \
../src/model/Field.cpp \
../src/model/Population.cpp \
../src/model/World.cpp 

OBJS += \
./src/model/Creature.o \
./src/model/Field.o \
./src/model/Population.o \
./src/model/World.o 

CPP_DEPS += \
./src/model/Creature.d \
./src/model/Field.d \
./src/model/Population.d \
./src/model/World.d 


# Each subdirectory must supply rules for building sources it contributes
src/model/%.o: ../src/model/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/wojtas/devel/include -I"/home/wojtas/workspace/project-world_old/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Viewer.cpp \
../src/WorldFrameListener.cpp \
../src/project-world.cpp 

OBJS += \
./src/Viewer.o \
./src/WorldFrameListener.o \
./src/project-world.o 

CPP_DEPS += \
./src/Viewer.d \
./src/WorldFrameListener.d \
./src/project-world.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/wojtas/devel/include -I"/home/wojtas/workspace/project-world_old/src" -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtSql -I/usr/include/qt4/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



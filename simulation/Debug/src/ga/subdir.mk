################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_UPPER_SRCS += \
../src/ga/GA1DArrayGenome.C \
../src/ga/GA1DBinStrGenome.C \
../src/ga/GA2DArrayGenome.C \
../src/ga/GA2DBinStrGenome.C \
../src/ga/GA3DArrayGenome.C \
../src/ga/GA3DBinStrGenome.C \
../src/ga/GAAllele.C \
../src/ga/GABaseGA.C \
../src/ga/GABin2DecGenome.C \
../src/ga/GABinStr.C \
../src/ga/GADCrowdingGA.C \
../src/ga/GADemeGA.C \
../src/ga/GAGenome.C \
../src/ga/GAIncGA.C \
../src/ga/GAList.C \
../src/ga/GAListBASE.C \
../src/ga/GAListGenome.C \
../src/ga/GAParameter.C \
../src/ga/GAPopulation.C \
../src/ga/GARealGenome.C \
../src/ga/GASStateGA.C \
../src/ga/GAScaling.C \
../src/ga/GASelector.C \
../src/ga/GASimpleGA.C \
../src/ga/GAStatistics.C \
../src/ga/GAStringGenome.C \
../src/ga/GATree.C \
../src/ga/GATreeBASE.C \
../src/ga/GATreeGenome.C \
../src/ga/gabincvt.C \
../src/ga/gaerror.C \
../src/ga/garandom.C 

OBJS += \
./src/ga/GA1DArrayGenome.o \
./src/ga/GA1DBinStrGenome.o \
./src/ga/GA2DArrayGenome.o \
./src/ga/GA2DBinStrGenome.o \
./src/ga/GA3DArrayGenome.o \
./src/ga/GA3DBinStrGenome.o \
./src/ga/GAAllele.o \
./src/ga/GABaseGA.o \
./src/ga/GABin2DecGenome.o \
./src/ga/GABinStr.o \
./src/ga/GADCrowdingGA.o \
./src/ga/GADemeGA.o \
./src/ga/GAGenome.o \
./src/ga/GAIncGA.o \
./src/ga/GAList.o \
./src/ga/GAListBASE.o \
./src/ga/GAListGenome.o \
./src/ga/GAParameter.o \
./src/ga/GAPopulation.o \
./src/ga/GARealGenome.o \
./src/ga/GASStateGA.o \
./src/ga/GAScaling.o \
./src/ga/GASelector.o \
./src/ga/GASimpleGA.o \
./src/ga/GAStatistics.o \
./src/ga/GAStringGenome.o \
./src/ga/GATree.o \
./src/ga/GATreeBASE.o \
./src/ga/GATreeGenome.o \
./src/ga/gabincvt.o \
./src/ga/gaerror.o \
./src/ga/garandom.o 

C_UPPER_DEPS += \
./src/ga/GA1DArrayGenome.d \
./src/ga/GA1DBinStrGenome.d \
./src/ga/GA2DArrayGenome.d \
./src/ga/GA2DBinStrGenome.d \
./src/ga/GA3DArrayGenome.d \
./src/ga/GA3DBinStrGenome.d \
./src/ga/GAAllele.d \
./src/ga/GABaseGA.d \
./src/ga/GABin2DecGenome.d \
./src/ga/GABinStr.d \
./src/ga/GADCrowdingGA.d \
./src/ga/GADemeGA.d \
./src/ga/GAGenome.d \
./src/ga/GAIncGA.d \
./src/ga/GAList.d \
./src/ga/GAListBASE.d \
./src/ga/GAListGenome.d \
./src/ga/GAParameter.d \
./src/ga/GAPopulation.d \
./src/ga/GARealGenome.d \
./src/ga/GASStateGA.d \
./src/ga/GAScaling.d \
./src/ga/GASelector.d \
./src/ga/GASimpleGA.d \
./src/ga/GAStatistics.d \
./src/ga/GAStringGenome.d \
./src/ga/GATree.d \
./src/ga/GATreeBASE.d \
./src/ga/GATreeGenome.d \
./src/ga/gabincvt.d \
./src/ga/gaerror.d \
./src/ga/garandom.d 


# Each subdirectory must supply rules for building sources it contributes
src/ga/%.o: ../src/ga/%.C
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/wojtas/devel/include -I"/home/wojtas/workspace/project-world_old/src" -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtSql -I/usr/include/qt4/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



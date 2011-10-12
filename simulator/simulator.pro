#-------------------------------------------------
#
# Project created by QtCreator 2011-10-12T19:16:00
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = simulator
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp src/model/World.cpp src/model/Field.cpp src/model/Population.cpp src/model/Creature.cpp \
           src/JSON/JSON.cpp src/JSON/JSONValue.cpp \
            src/ga/GA1DArrayGenome.C   src/ga/GA3DArrayGenome.C   src/ga/GABin2DecGenome.C  src/ga/GADemeGA.C  \
            src/ga/GAListBASE.C    src/ga/GAPopulation.C  src/ga/GASelector.C    src/ga/GAStringGenome.C \
            src/ga/GA1DBinStrGenome.C  src/ga/GA3DBinStrGenome.C  src/ga/gabincvt.C         src/ga/gaerror.C   src/ga/GAList.C \
        src/ga/garandom.C      src/ga/GASimpleGA.C    src/ga/GATreeBASE.C \
        src/ga/GA2DArrayGenome.C   src/ga/GAAllele.C          src/ga/GABinStr.C         src/ga/GAGenome.C  src/ga/GAListGenome.C \
  src/ga/GARealGenome.C  src/ga/GASStateGA.C    src/ga/GATree.C \
    src/ga/GA2DBinStrGenome.C  src/ga/GABaseGA.C          src/ga/GADCrowdingGA.C    src/ga/GAIncGA.C   src/ga/GAParameter.C  \
  src/ga/GAScaling.C     src/ga/GAStatistics.C  src/ga/GATreeGenome.C

INCLUDEPATH += src/

LIBS += -llog4cxx

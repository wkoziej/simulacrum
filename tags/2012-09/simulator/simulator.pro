#-------------------------------------------------
#
# Project created by QtCreator 2011-10-12T19:16:00
#
#-------------------------------------------------

QT       += core sql gui xml

TARGET = simulator
CONFIG   -= app_bundle
CONFIG += app
TEMPLATE = app


SOURCES += main.cpp src/model/Activity.cpp src/model/Article.cpp \
					src/model/World.cpp src/model/Field.cpp \
					src/model/Population.cpp src/model/Creature.cpp \
					src/model/Market.cpp src/model/ModelHelpers.cpp src/model/Recipe.cpp \
           			src/JSON/JSON.cpp src/JSON/JSONValue.cpp \
            src/ga/GA1DArrayGenome.C   src/ga/GA3DArrayGenome.C   src/ga/GABin2DecGenome.C  src/ga/GADemeGA.C  \
            src/ga/GAListBASE.C    src/ga/GAPopulation.C  src/ga/GASelector.C    src/ga/GAStringGenome.C \
            src/ga/GA1DBinStrGenome.C  src/ga/GA3DBinStrGenome.C  src/ga/gabincvt.C         src/ga/gaerror.C   src/ga/GAList.C \
        src/ga/garandom.C      src/ga/GASimpleGA.C    src/ga/GATreeBASE.C \
        src/ga/GA2DArrayGenome.C   src/ga/GAAllele.C          src/ga/GABinStr.C         src/ga/GAGenome.C  src/ga/GAListGenome.C \
  src/ga/GARealGenome.C  src/ga/GASStateGA.C    src/ga/GATree.C \
    src/ga/GA2DBinStrGenome.C  src/ga/GABaseGA.C          src/ga/GADCrowdingGA.C    src/ga/GAIncGA.C   src/ga/GAParameter.C  \
  src/ga/GAScaling.C     src/ga/GAStatistics.C  src/ga/GATreeGenome.C \
    src/visual/VisualizationController.cpp \
    src/visual/StyleSettingsWidget.cpp \
    src/visual/SimulatorGameLogic.cpp \
    src/visual/MainMenu.cpp \
    src/visual/FieldVisualizationWidget.cpp src/StateSaver.cpp

INCLUDEPATH += src/ /usr/local/include/QtOgre /usr/include/qt4/QtCore /usr/include/qt4/QtGui /usr/include/qt4/QtXml /usr/include/OGRE

LIBS += -llog4cxx -lQtOgre -lOgreMain

target.files += world.json log.properties resources.cfg plugins_d.cfg project-world.db settings.ini
target.files += media
target.path = ../sim_bin
INSTALLS += target

DESTDIR = ../sim_bin

HEADERS += \
    src/visual/VisualizationController.h \
    src/visual/StyleSettingsWidget.h \
    src/visual/SimulatorGameLogic.h \
    src/visual/MainMenu.h \
    src/visual/FieldVisualizationWidget.h \
    src/model/Recipe.h \
    src/model/Activity.h \
    src/model/Creature.h

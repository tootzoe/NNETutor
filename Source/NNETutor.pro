




#TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

#
#
PRJNAMETOOT = NNETutor
DEFINES += "NNETUTOR_API="
DEFINES += "NNETUTOR_API(...)="
#
DEFINES += "UCLASS()=NNETUTOR_API"
DEFINES += "UCLASS(...)=NNETUTOR_API"
#
# this is true during development with unreal-editor...

DEFINES += "WITH_EDITORONLY_DATA=1"

## this project only




INCLUDEPATH += ../Intermediate/Build/Win64/UnrealEditor/Inc/$$PRJNAMETOOT/UHT
INCLUDEPATH += $$PRJNAMETOOT/Public $$PRJNAMETOOT/Private
#INCLUDEPATH += ../Plugins/NNEPostProcessing/Source/NNEPostProcessing/Public
# we should follow UE project struct to include files, start from prj.Build.cs folder
#
#  The Thirdparty libs
#
#
#
include(defs.pri)
include(inc.pri)

#
#

DISTFILES += \
    NNETutor.Target.cs \
    NNETutor/NNETutor.Build.cs \
    NNETutorEditor.Target.cs

HEADERS += \
    NNETutor/NNETutor.h \
    NNETutor/Public/NNEBasicInfo.h

SOURCES += \
    NNETutor/NNETutor.cpp \
    NNETutor/Private/NNEBasicInfo.cpp

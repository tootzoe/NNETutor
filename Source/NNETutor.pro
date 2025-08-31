




#TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

#
# below one line project name need to be lowercaes
#### Module 1
PRJMODULE1  = NNETutor
DEFINES += "NNETUTOR_API=__declspec(dllexport)"
#
INCLUDEPATH += ../Intermediate/Build/Win64/UnrealEditor/Inc/$$PRJMODULE1/UHT
INCLUDEPATH += $$PRJMODULE1 $$PRJMODULE1/Public $$PRJMODULE1/Private
#### Module 2
# PRJMODULE2  = StrategyGameLoadingScreen
# DEFINES += "STRATEGYGAMELOADINGSCREEN_API=__declspec(dllexport)"
# #
# INCLUDEPATH += ../Intermediate/Build/Win64/UnrealEditor/Inc/$$PRJMODULE2/UHT
# INCLUDEPATH += $$PRJMODULE2 $$PRJMODULE2/Public $$PRJMODULE2/Private
####


#
# this is true during development with unreal-editor...

DEFINES += "WITH_EDITORONLY_DATA=1"

## this project only
DEFINES += PLATFORM_ANDROID

##

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
## this project only
##
#### PLUGINSROOT 1
PLUGINSROOT = ../Plugins
###
###
#### Start Plugins Module 1 Start
PLUGINNAME1  = NNERuntimeOpenVINO
PLUGINMODULE1  = NNERuntimeOpenVINO
DEFINES += "NNERUNTIMEOPENVINO_API=__declspec(dllexport)"
#
INCLUDEPATH += $$PLUGINSROOT/$$PLUGINNAME1/Intermediate/Build/Win64/UnrealEditor/Inc/$$PLUGINMODULE1/UHT
INCLUDEPATH += $$PLUGINSROOT/$$PLUGINNAME1/Source/$$PLUGINMODULE1
INCLUDEPATH += $$PLUGINSROOT/$$PLUGINNAME1/Source/$$PLUGINMODULE1/Public
INCLUDEPATH += $$PLUGINSROOT/$$PLUGINNAME1/Source/$$PLUGINMODULE1/Private
########## End Plugins Module 1 End
#
PLUGINNAME2  = NNERuntimeOpenVINO
PLUGINMODULE2  = NNERuntimeOpenVINOEditor
DEFINES += "NNERUNTIMEOPENVINOEDITOR_API=__declspec(dllexport)"
#
INCLUDEPATH += $$PLUGINSROOT/$$PLUGINNAME2/Intermediate/Build/Win64/UnrealEditor/Inc/$$PLUGINMODULE2/UHT
INCLUDEPATH += $$PLUGINSROOT/$$PLUGINNAME2/Source/$$PLUGINMODULE2
INCLUDEPATH += $$PLUGINSROOT/$$PLUGINNAME2/Source/$$PLUGINMODULE2/Public
INCLUDEPATH += $$PLUGINSROOT/$$PLUGINNAME2/Source/$$PLUGINMODULE2/Private
########## End Plugins Module 2 End
#
#
PLUGINNAME3  = NNERuntimeOpenVINO/Source/ThirdParty
PLUGINMODULE3  = OpenVino
DEFINES += "OPENVINO_API=__declspec(dllexport)"
#
INCLUDEPATH += $$PLUGINSROOT/$$PLUGINNAME3/Intermediate/Build/Win64/UnrealEditor/Inc/$$PLUGINMODULE3/UHT
TOOTZ = $$PLUGINSROOT/$$PLUGINNAME3/$$PLUGINMODULE3/Internal/openvino/c
INCLUDEPATH += $$PLUGINSROOT/$$PLUGINNAME3/$$PLUGINMODULE3/Internal/openvino/c
INCLUDEPATH += $$PLUGINSROOT/$$PLUGINNAME3/$$PLUGINMODULE3/Internal/openvino/c/auto
INCLUDEPATH += $$PLUGINSROOT/$$PLUGINNAME3/$$PLUGINMODULE3/Internal/openvino/c/gpu
#INCLUDEPATH += $$PLUGINSROOT/$$PLUGINNAME3/Source/$$PLUGINMODULE2/Public
#INCLUDEPATH += $$PLUGINSROOT/$$PLUGINNAME3/Source/$$PLUGINMODULE2/Private
########## End Plugins Module 3 End
#


######## for debug .pro file
#QMAKE_LFLAGS += -s
#QMAKE_CXXFLAGS += "-std=c++11"
#message($$system(date -I))
#TOOTBDATE = '\\"$$system(date -I\'minute\')\\"'
#DEFINES += TOOTBUILDDATE=\"$${TOOTBDATE}\"
#message($${TOOTDebugStr})
##
##
## this project only
INCLUDEPATH += $$UESRCROOT/Runtime/Renderer/Private
##

##
#
#

HEADERS += \
    NNETutor/NNETutor.h \
    NNETutor/Public/NNEBasicInfo.h \
    NNETutor/Public/NeuralPostProcessingCS.h \
    NNETutor/Public/NeuralPostProcessingViewExtension.h \
    NNETutor/Shaders/NeuralPostProcessing.usf

SOURCES += \
    NNETutor/NNETutor.cpp \
    NNETutor/Private/NNEBasicInfo.cpp \
    NNETutor/Private/NeuralPostProcessingCS.cpp \
    NNETutor/Private/NeuralPostProcessingViewExtension.cpp

DISTFILES += \
    ../NNETutor.uproject \
    NNETutor.Target.cs \
    NNETutor/NNETutor.Build.cs \
    NNETutorEditor.Target.cs

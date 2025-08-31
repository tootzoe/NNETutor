




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
## INCLUDEPATH += $$UESRCROOT/Runtime/Renderer/Private
PLUGINSROOT = ../Plugins
#INCLUDEPATH += $$PLUGINSROOT/UIExtension/Source/Public
#
## this project only
INCLUDEPATH += $$UESRCROOT/Runtime/Renderer/Private
##

##
#
#

HEADERS += \
    AdventurePrj/AdventurePrj.h \
    AdventurePrj/Public/AdventureCharacter.h \
    AdventurePrj/Public/AdventureGM.h \
    AdventurePrj/Public/Data/EquippableToolDefinition.h \
    AdventurePrj/Public/Data/ItemData.h \
    AdventurePrj/Public/Data/ItemDefinition.h \
    AdventurePrj/Public/EquippableToolBase.h \
    AdventurePrj/Public/InventoryComp.h \
    AdventurePrj/Public/PickupBase.h \
    AdventurePrj/Public/Projectile/FirstPersionProjectile.h \
    AdventurePrj/Public/Tools/DartLauncher.h \
    AdventurePrj/Public/UI/DragWidget.h \
    AdventurePrj/Public/UI/HUDLayout.h \
    AdventurePrj/Public/UI/TootWindow.h \
    NNETutor/NNETutor.h \
    NNETutor/Public/NNEBasicInfo.h \
    NNETutor/Public/NeuralPostProcessingCS.h \
    NNETutor/Public/NeuralPostProcessingViewExtension.h \
    NNETutor/Shaders/NeuralPostProcessing.usf

SOURCES += \
    AdventurePrj/AdventurePrj.cpp \
    AdventurePrj/Private/AdventureCharacter.cpp \
    AdventurePrj/Private/AdventureGM.cpp \
    AdventurePrj/Private/Data/EquippableToolDefinition.cpp \
    AdventurePrj/Private/Data/ItemDefinition.cpp \
    AdventurePrj/Private/EquippableToolBase.cpp \
    AdventurePrj/Private/InventoryComp.cpp \
    AdventurePrj/Private/PickupBase.cpp \
    AdventurePrj/Private/Projectile/FirstPersionProjectile.cpp \
    AdventurePrj/Private/Tools/DartLauncher.cpp \
    AdventurePrj/Private/UI/DragWidget.cpp \
    AdventurePrj/Private/UI/HUDLayout.cpp \
    AdventurePrj/Private/UI/TootWindow.cpp \
    NNETutor/NNETutor.cpp \
    NNETutor/Private/NNEBasicInfo.cpp \
    NNETutor/Private/NeuralPostProcessingCS.cpp \
    NNETutor/Private/NeuralPostProcessingViewExtension.cpp

DISTFILES += \
    AdventurePrj.Target.cs \
    AdventurePrj/AdventurePrj.Build.cs \
    AdventurePrjEditor.Target.cs \
    NNETutor.Target.cs \
    NNETutor/NNETutor.Build.cs \
    NNETutorEditor.Target.cs

// Copyright Epic Games, Inc. All Rights Reserved.

#include "NNETutor.h"
#include "Modules/ModuleManager.h"


#include "Interfaces/IPluginManager.h"
#include "ShaderCore.h"



void FNNETutorModule::StartupModule()
{
    UE_LOG(LogTemp, Warning, TEXT("My NNETutor app StartupModule......."));

    FString moduleDir = FPaths::GameSourceDir() + TEXT("NNETutor");

   // const TSharedPtr<IPlugin> tmpPlugin = IPluginManager::Get().FindPlugin(TEXT("NE"))

    AddShaderSourceDirectoryMapping(TEXT("/NNETutor") , FPaths::Combine(moduleDir, TEXT("Shaders")));


}

void FNNETutorModule::ShutdownModule()
{
  UE_LOG(LogTemp, Warning, TEXT("My NNETutor app ShutdownModule......."));
}



IMPLEMENT_PRIMARY_GAME_MODULE(  FNNETutorModule /*FDefaultGameModuleImpl*/, NNETutor, "NNETutor" );

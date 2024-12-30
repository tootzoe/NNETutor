/**************************************************************************
**   Copyright @ 2024 TOOTzoe.com
**   Special keywords: thor 12/30/2024 2024
**   Environment variables:
**
**
**
**   E-mail : toot@tootzeo.com
**   Tel    : 13802205042
**   Website: http://www.tootzoe.com
**
**************************************************************************/



#ifdef Q_CC_MSVC
#pragma execution_character_set("UTF-8")
#endif


//------------------------------------------------------------------------


#include "NeuralPostProcessingCS.h"

void FNeuralPostProcessingPrepareInputCS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& InParameters, FShaderCompilerEnvironment& OutEnvironment)
{

    FGlobalShader::ModifyCompilationEnvironment(InParameters, OutEnvironment);
    OutEnvironment.SetDefine(TEXT("THREAD_GROUP_SIZE"), NEURAL_POST_PROCESSING_THREAD_GROUP_SIZE);
}

void FNeuralPostProcessingProcessOutputPS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& InParameters, FShaderCompilerEnvironment& OutEnvironment)
{
    FGlobalShader::ModifyCompilationEnvironment(InParameters, OutEnvironment);
    OutEnvironment.SetDefine(TEXT("THREAD_GROUP_SIZE"), NEURAL_POST_PROCESSING_THREAD_GROUP_SIZE);
}

IMPLEMENT_GLOBAL_SHADER(FNeuralPostProcessingPrepareInputCS, "/NNETutor/NeuralPostProcessing.usf", "PrepareInput", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FNeuralPostProcessingProcessOutputPS, "/NNETutor/NeuralPostProcessing.usf", "ProcessOutput", SF_Pixel);

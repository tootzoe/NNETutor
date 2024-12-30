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


#pragma once

#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "RenderGraphUtils.h"

#define NEURAL_POST_PROCESSING_THREAD_GROUP_SIZE 32

class NNETUTOR_API FNeuralPostProcessingPrepareInputCS : public FGlobalShader
{
    DECLARE_GLOBAL_SHADER(FNeuralPostProcessingPrepareInputCS)
    SHADER_USE_PARAMETER_STRUCT(FNeuralPostProcessingPrepareInputCS, FGlobalShader)

public:

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER(int32, Width)
        SHADER_PARAMETER(int32, Height)
        SHADER_PARAMETER_RDG_TEXTURE(Texture2D, InputTexture)
        SHADER_PARAMETER_SAMPLER(SamplerState, InputTextureSampler)
        SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<float>, InputBuffer)
    END_SHADER_PARAMETER_STRUCT()

    static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& InParameters, FShaderCompilerEnvironment& OutEnvironment);
};

class NNETUTOR_API FNeuralPostProcessingProcessOutputPS : public FGlobalShader
{
    DECLARE_GLOBAL_SHADER(FNeuralPostProcessingProcessOutputPS)
    SHADER_USE_PARAMETER_STRUCT(FNeuralPostProcessingProcessOutputPS, FGlobalShader)

public:

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER(int32, Width)
        SHADER_PARAMETER(int32, Height)
        SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<float>, OutputBuffer)
        RENDER_TARGET_BINDING_SLOTS()
    END_SHADER_PARAMETER_STRUCT()

    static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& InParameters, FShaderCompilerEnvironment& OutEnvironment);
};

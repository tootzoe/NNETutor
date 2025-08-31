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


#include "NeuralPostProcessingViewExtension.h"

#include "NNE.h"
#include "NeuralPostProcessingCS.h"
#include "PostProcess/PostProcessing.h"
#include "PixelShaderUtils.h"


 using namespace UE::NNE;


FNeuralPostProcessingViewExtension::FNeuralPostProcessingViewExtension(const FAutoRegister &AutoRegister) :
    FSceneViewExtensionBase(AutoRegister)
    ,LastInputSize (-1, -1)
{

}

bool FNeuralPostProcessingViewExtension::SetModel(FString RuntimeName, TObjectPtr<UNNEModelData> ModelData)
{
    if(!ModelData){
        ModelInstance.Reset();
        LastInputSize = FIntPoint(-1, -1);
        return true;
    }



    TWeakInterfacePtr<INNERuntimeRDG> rtRDG = GetRuntime<INNERuntimeRDG>(RuntimeName);
    if(!rtRDG.IsValid()){
        UE_LOG(LogTemp, Error, TEXT("No RT RDG runtie '%s' found.....") , *RuntimeName);
        return false;
    }

    TSharedPtr<IModelRDG> tmpModel = rtRDG->CreateModelRDG(ModelData);
    if (!tmpModel.IsValid()) {
        UE_LOG(LogTemp, Error, TEXT("Could NOT create RDG model.....") );
        return false;
    }

    ModelInstance = tmpModel->CreateModelInstanceRDG();
    if (!ModelInstance.IsValid()) {
        UE_LOG(LogTemp, Error, TEXT("Could NOT create RDG model instance.....") );
        return false;
    }

    return true;
}

void FNeuralPostProcessingViewExtension::PrePostProcessPass_RenderThread(FRDGBuilder &GraphBuilder, const FSceneView &View, const FPostProcessingInputs &Inputs)
{

    check(IsInRenderingThread());
    check(View.bIsViewInfo);

    if(!ModelInstance.IsValid()){
        return;
    }

    checkf(ModelInstance->GetInputTensorDescs().Num() == 1 , TEXT("This Neural Post Processing requires models with a single input tensor......"));
    FSymbolicTensorShape inputShape = ModelInstance->GetInputTensorDescs()[0].GetShape();


    checkf(inputShape.Rank() == 4, TEXT("Neural Post Processing requires models with input shape 1 x 3 x height x width!"));
    checkf(inputShape.GetData()[0] == 1, TEXT("Neural Post Processing requires models with input shape 1 x 3 x height x width!"));
    checkf(inputShape.GetData()[1] == 3, TEXT("Neural Post Processing requires models with input shape 1 x 3 x height x width!"));
    checkf(inputShape.GetData()[2] == -1, TEXT("Neural Post Processing requires models with input shape 1 x 3 x height x width!"));
    checkf(inputShape.GetData()[3] == -1, TEXT("Neural Post Processing requires models with input shape 1 x 3 x height x width!"));

    FIntPoint textureSz = (*Inputs.SceneTextures)->SceneColorTexture->Desc.Extent;

    if (LastInputSize.X != textureSz.X || LastInputSize.Y != textureSz.Y) {
        TArray<FTensorShape> inputShapes  = { FTensorShape::Make({1,3, (uint32)textureSz.Y , (uint32)textureSz.X})};
        ModelInstance->SetInputTensorShapes(inputShapes);
        LastInputSize = textureSz;
    }

    checkf(ModelInstance->GetInputTensorShapes().Num() == 1 , TEXT("Neural Post Processing requires models with a single output tensor...."));

    FTensorShape OutputShape = ModelInstance->GetOutputTensorShapes()[0];
    checkf(OutputShape.Rank() == 4, TEXT("Neural Post Processing requires models with output shape 1 x 3 x height x width!"));
    checkf(OutputShape.GetData()[0] == 1, TEXT("Neural Post Processing requires models with output shape 1 x 3 x height x width!"));
    checkf(OutputShape.GetData()[1] == 3, TEXT("Neural Post Processing requires models with output shape 1 x 3 x height x width!"));
    checkf(OutputShape.GetData()[2] == textureSz.Y, TEXT("Neural Post Processing requires models with output height == input height!"));
    checkf(OutputShape.GetData()[3] == textureSz.X, TEXT("Neural Post Processing requires models with output width == input width!"));
    //
    FRDGBufferDesc inputBuffDesc = FRDGBufferDesc::CreateBufferDesc(sizeof(float), textureSz.X * textureSz.Y * 3);
    FRDGBufferRef inputBuff = GraphBuilder.CreateBuffer(inputBuffDesc, *FString("NeuralPostProcessing::InputBuffer"));
    FRDGBufferUAVRef inputBuffUAV = GraphBuilder.CreateUAV(FRDGBufferUAVDesc(inputBuff, PF_R32_FLOAT));
    //
    FNeuralPostProcessingPrepareInputCS::FParameters* prepareInputParameters = GraphBuilder.AllocParameters<FNeuralPostProcessingPrepareInputCS::FParameters>();
    prepareInputParameters->InputTexture = (*Inputs.SceneTextures)->SceneColorTexture;
    prepareInputParameters->InputTextureSampler = TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp , AM_Clamp>::GetRHI();
    prepareInputParameters->Width = textureSz.X;
    prepareInputParameters->Height = textureSz.Y;
    prepareInputParameters->InputBuffer = inputBuffUAV;
    //
    FIntVector prepareInputThreadGroupCount = FIntVector(FMath::DivideAndRoundUp(textureSz.X ,NEURAL_POST_PROCESSING_THREAD_GROUP_SIZE),
                                                         FMath::DivideAndRoundUp(textureSz.Y ,NEURAL_POST_PROCESSING_THREAD_GROUP_SIZE),
                                                         1);
    FGlobalShaderMap* globalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
    TShaderMapRef<FNeuralPostProcessingPrepareInputCS> prepareInputShader(globalShaderMap);
    FComputeShaderUtils::AddPass(
                GraphBuilder,
                RDG_EVENT_NAME("NeuralPostProcessing.PrepareInput"),
                ERDGPassFlags::Compute | ERDGPassFlags::NeverCull,
                prepareInputShader,
                prepareInputParameters,
                prepareInputThreadGroupCount
                );
    //
    FRDGBufferDesc outputBufferDesc = FRDGBufferDesc::CreateBufferDesc(sizeof(float), textureSz.X * textureSz.Y * 3);
    FRDGBufferRef outputBuffer = GraphBuilder.CreateBuffer(outputBufferDesc, *FString("NeuralPostProcessing::OutputBuffer"));
    FRDGBufferUAVRef outputBufferUAV = GraphBuilder.CreateUAV(FRDGBufferUAVDesc(outputBuffer, PF_R32_FLOAT));

    //
    TArray<FTensorBindingRDG> inputBindings;
    TArray<FTensorBindingRDG> outputBindings;
    FTensorBindingRDG& inp = inputBindings.Emplace_GetRef();
    FTensorBindingRDG& outp = outputBindings.Emplace_GetRef();
    inp.Buffer = inputBuff;
    outp.Buffer = outputBuffer;
    ModelInstance->EnqueueRDG(GraphBuilder, inputBindings , outputBindings);

    //
    const FIntRect viewport = static_cast<const FViewInfo&>(View).ViewRect;
    FScreenPassTexture sceneCr( (*Inputs.SceneTextures)->SceneColorTexture , viewport);
    FNeuralPostProcessingProcessOutputPS::FParameters* processOutputPrarmeters = GraphBuilder.AllocParameters<FNeuralPostProcessingProcessOutputPS::FParameters>();
    processOutputPrarmeters->Width = textureSz.X;
    processOutputPrarmeters->Height = textureSz.Y;
    processOutputPrarmeters->OutputBuffer = outputBufferUAV;
    processOutputPrarmeters->RenderTargets[0] = FRenderTargetBinding(sceneCr.Texture, ERenderTargetLoadAction::ENoAction);
    //
    TShaderMapRef<FNeuralPostProcessingProcessOutputPS> writeOutputShader(globalShaderMap);
    FPixelShaderUtils::AddFullscreenPass(
                GraphBuilder,
                globalShaderMap,
                RDG_EVENT_NAME("NeuralPostProcessing.ProcessOutput"),
                writeOutputShader,
                processOutputPrarmeters,
                viewport
                );




}

bool UNeuralPostProcessing::SetModel(FString RuntimeName, UNNEModelData *ModelData)
{
    if (!mNeuralPPViewExt.IsValid()) {
        mNeuralPPViewExt = FSceneViewExtensions::NewExtension<FNeuralPostProcessingViewExtension>();

    }

    return mNeuralPPViewExt->SetModel(RuntimeName,TObjectPtr<UNNEModelData>(ModelData));
}


void FNeuralPostProcessingViewExtension::PreRenderViewFamily_RenderThread(FRDGBuilder &GraphBuilder, FSceneViewFamily &InViewFamily)
{
}

void FNeuralPostProcessingViewExtension::PreRenderView_RenderThread(FRDGBuilder &GraphBuilder, FSceneView &InView)
{
}

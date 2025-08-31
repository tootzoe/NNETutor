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

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "SceneViewExtension.h"

#include "NNEModelData.h"
#include "NNERuntimeRDG.h"

#include "NeuralPostProcessingViewExtension.generated.h"

/**
 *
 */

class   FNeuralPostProcessingViewExtension : public FSceneViewExtensionBase
{

 public:
    FNeuralPostProcessingViewExtension(const FAutoRegister& AutoRegister);
public:

    bool SetModel(FString RuntimeName, TObjectPtr<UNNEModelData> ModelData);

public:

    virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {}
    virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
    virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {}
  //  virtual void PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override {}
  //  virtual void PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override {}
    virtual void PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs) override;

    // ISceneViewExtension interface
    virtual void PreRenderViewFamily_RenderThread(FRDGBuilder &GraphBuilder, FSceneViewFamily &InViewFamily) override;
    virtual void PreRenderView_RenderThread(FRDGBuilder &GraphBuilder, FSceneView &InView) override;


 private:

    TSharedPtr<UE::NNE::IModelInstanceRDG> ModelInstance;
    FIntPoint LastInputSize;



};



UCLASS( Blueprintable , Category = "TOOTzoe" )
class NNETUTOR_API UNeuralPostProcessing : public UObject
{
   GENERATED_BODY()

   public:
  UFUNCTION(BlueprintCallable, Category = "TOOT")
  bool SetModel(FString RuntimeName , UNNEModelData* ModelData);

   private:

TSharedPtr<FNeuralPostProcessingViewExtension> mNeuralPPViewExt;

   };

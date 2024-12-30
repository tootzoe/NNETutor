// Fill out your copyright notice in the Description page of Project Settings.


#include "NNEBasicInfo.h"


#include "NNE.h"

#include "NNERuntime.h"
#include "NNERuntimeCPU.h"

#include "NNEModelData.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Misc/Paths.h"


void UNNEBasicInfo::FetchAllNNE_Runtimes()
{

    UE_LOG(LogTemp, Warning, TEXT("%hs") , __func__);

   // TArrayView< TWeakInterfacePtr<INNERuntime > > tmpINNE = UE::NNE::GetAllRuntimeNames()

    // TArrayView<const FString> names = UE::NNE::GetAllRuntimeNames ();

    // for  ( const FString inneName : names ) {
    //         UE_LOG(LogTemp, Warning, TEXT("NNE name:  %s  "), *inneName);
    // }

     FString p =  FPaths::GameSourceDir();  // =  D:/workprj/uePrj/NNETutor/Source/

     UE_LOG(LogTemp, Warning, TEXT(" p == %s") , *p );

}

void UNNEBasicInfo::LoadModel_MNIST8()
{

    UE_LOG(LogTemp, Warning, TEXT("Load Mnist-8........"));

    TObjectPtr<UNNEModelData> modelDat = LoadObject<UNNEModelData>(GetTransientPackage() , TEXT("/Game/OnnxModel/model_mnist-8.model_mnist-8"));


    if(modelDat){
        UE_LOG(LogTemp, Warning, TEXT("Model Data loaded.... %s") , *modelDat->GetName());
    }
    else{
        UE_LOG(LogTemp, Error, TEXT("Failed to load model data.....please check the static path to your asset....")  );
    }

    TWeakInterfacePtr<INNERuntimeCPU> rtCpu = UE::NNE::GetRuntime<INNERuntimeCPU>(FString("NNERuntimeORTCpu"));

    if (!rtCpu.IsValid()) {
        UE_LOG(LogTemp, Error, TEXT("INNE Runtime Cpu invalid....."));
        return;
    }


    TSharedPtr<UE::NNE::IModelCPU> model  = rtCpu->CreateModelCPU (modelDat);

    if(!model.IsValid()){
         UE_LOG(LogTemp, Error, TEXT("Failed to create cpu model....."));
        return;
    }

    TSharedPtr<UE::NNE::IModelInstanceCPU> modelInst = model->CreateModelInstanceCPU();
    if (!modelInst.IsValid()) {
        UE_LOG(LogTemp, Error, TEXT("Failed to create cpu model inst....."));
    }


    using namespace UE::NNE;

    TConstArrayView<UE::NNE::FTensorDesc> inputTensorDesc = modelInst->GetInputTensorDescs();
    checkf(inputTensorDesc.Num() == 1 , TEXT("The current example supports only models with a single input tensor...."));
    UE::NNE::FSymbolicTensorShape symbolicInputTensorShape = inputTensorDesc[0].GetShape();
    checkf(symbolicInputTensorShape.IsConcrete(), TEXT("The current example supports only models without variable input tensor dimensions"));
    TArray<UE::NNE::FTensorShape> inputTensorShapes = { FTensorShape::MakeFromSymbolic(symbolicInputTensorShape) };

    modelInst->SetInputTensorShapes(inputTensorShapes);

    TConstArrayView<UE::NNE::FTensorDesc> outputTensorDesc = modelInst->GetOutputTensorDescs();
    checkf(outputTensorDesc.Num() == 1 , TEXT("The current example supports only models with a single output tensor...."));
    UE::NNE::FSymbolicTensorShape symbolicOutputTensorShape = outputTensorDesc[0].GetShape();
    checkf(symbolicOutputTensorShape.IsConcrete(), TEXT("The current example supports only models without variable output tensor dimensions"));
    TArray<UE::NNE::FTensorShape> outputTensorShapes = { FTensorShape::MakeFromSymbolic(symbolicOutputTensorShape) };


    TArray<float> myInputData;
    myInputData.SetNumZeroed(inputTensorShapes[0].Volume());
    TArray<FTensorBindingCPU> inputBindings;
    inputBindings.SetNumZeroed(1);
    inputBindings[0].Data = myInputData.GetData();
    inputBindings[0].SizeInBytes = myInputData.Num() * sizeof(float);
    //
    TArray<float> myOutputData;
    myOutputData.SetNumZeroed(outputTensorShapes[0].Volume());
    TArray<FTensorBindingCPU> outputBindings;
    outputBindings.SetNumZeroed(1);
    outputBindings[0].Data = myOutputData.GetData();
    outputBindings[0].SizeInBytes = myOutputData.Num() * sizeof(float);



   // myOutputData.SetNumZeroed(inputTensorShapes[0].Volume());

    UE_LOG(LogTemp, Error, TEXT(" Everythings goes well here....."));
}

void UNNEBasicInfo::LazyLoadModel_MNIST8()
{
    //FSoftClassPath mnist8Path(TEXT("/Game/OnnxModel/model_mnist-8.model_mnist-8"));
    FSoftObjectPath mnist8Path(TEXT("/Game/OnnxModel/model_mnist-8.model_mnist-8"));

    TObjectPtr<UNNEModelData> myModelDat = Cast<UNNEModelData>(  mnist8Path.ResolveObject() );

    if(!myModelDat) {
        UE_LOG(LogTemp, Warning, TEXT("myModelDat null..."));
    }else
    {
         UE_LOG(LogTemp, Warning, TEXT("myModelDat Not null..."));
    }


    // mLazyModelMnist8 =
    mLazyModelMnist8 =  TSoftObjectPtr<UNNEModelData>  (mnist8Path);
    if(mLazyModelMnist8.IsValid()){
        UE_LOG(LogTemp, Warning, TEXT("LazyModel data is ok...."));
    }else{
        UE_LOG(LogTemp, Warning, TEXT("LazyModel data invalid, try load it...."));

    }

   // mLazyModelMnist8.Reset();
   //  UE_LOG(LogTemp, Warning, TEXT("Reset mLazyModelMnist8...."));

    // mLazyModelMnist8 =  TSoftObjectPtr<UNNEModelData>  (mnist8Path);

     if(mLazyModelMnist8.IsValid()){
         UE_LOG(LogTemp, Warning, TEXT("LazyModel data is ok...."));
     }else{
         UE_LOG(LogTemp, Warning, TEXT("LazyModel data invalid, try load it...."));

         UAssetManager::GetStreamableManager().RequestAsyncLoad(mLazyModelMnist8.ToSoftObjectPath(),
             FStreamableDelegate::CreateUObject(this, &UNNEBasicInfo::lazyLoadHandler)
          );

     }

   // mLazyModelMnist8 = CastChecked<UNNEModelData>(mnist8Path  .TryLoad());


   //


    //if(mLazyModelMnist8.IsNull()){
      // UE_LOG(LogTemp, Warning, TEXT("mLazyModelMnist8   IsNull now...."));


    // }else{
    //     UE_LOG(LogTemp, Warning, TEXT("mLazyModelMnist8 is Not null ...."));


    // }
}

void UNNEBasicInfo::Check_MNIST8()
{
    if (mLazyModelMnist8.IsNull()) {
        UE_LOG(LogTemp, Warning, TEXT("mLazyModelMnist8 Is Null......"));
    }else{
         UE_LOG(LogTemp, Warning, TEXT("mLazyModelMnist8 Is NOT Null......"));
    }

    if (mLazyModelMnist8.IsValid()) {
        UE_LOG(LogTemp, Warning, TEXT("mLazyModelMnist8 valid......"));
    }else{
         UE_LOG(LogTemp, Warning, TEXT("mLazyModelMnist8 Invalid......"));
    }

}

void UNNEBasicInfo::lazyLoadHandler()
{
    UE_LOG(LogTemp, Warning, TEXT("%hs called.........") , __func__);

   // mLazyModelMnist8 =
}

void UNNEBasicInfo::MNIST8_RunORTCpu()
{
    TWeakInterfacePtr<INNERuntimeCPU> rtCpu = UE::NNE::GetRuntime<INNERuntimeCPU>(FString("NNERuntimeORTCpu"));

    if (!rtCpu.IsValid()) {
        UE_LOG(LogTemp, Error, TEXT("INNE Runtime Cpu invalid....."));
        return;
    }



}





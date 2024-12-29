// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NNEBasicInfo.generated.h"

/**
 * 
 */
UCLASS( Blueprintable )
class NNETUTOR_API UNNEBasicInfo : public UObject
{
	GENERATED_BODY()
	
 public:
     UFUNCTION(BlueprintCallable, Category = "TOOT")
     void FetchAllNNE_Runtimes();
	
	
    UFUNCTION(BlueprintCallable, Category = "TOOT")
    void LoadModel_MNIST8();

    UFUNCTION(BlueprintCallable, Category = "TOOT")
    void LazyLoadModel_MNIST8();

    UFUNCTION(BlueprintCallable, Category = "TOOT")
    void Check_MNIST8();

    UFUNCTION(BlueprintCallable, Category = "TOOT")
    void MNIST8_RunORTCpu();





  private:

    void lazyLoadHandler();

    UPROPERTY(  )
    TSoftObjectPtr<class UNNEModelData> mLazyModelMnist8;

};

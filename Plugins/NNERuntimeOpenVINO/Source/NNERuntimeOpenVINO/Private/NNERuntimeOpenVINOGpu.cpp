/*******************************************************************************
* Copyright (C) 2025 Intel Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom
* the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
* OR OTHER DEALINGS IN THE SOFTWARE.
*
* SPDX-License-Identifier: MIT
******************************************************************************/

#include "NNERuntimeOpenVINOGpu.h"

#include "Memory/SharedBuffer.h"
#include "NNE.h"
#include "NNEModelData.h"
#include "NNERuntimeOpenVINOCommon.h"
#include "Serialization/MemoryWriter.h"

#include "openvino/c/ov_prepostprocess.h"
#include "openvino/c/ov_tensor.h"

FGuid UNNERuntimeOpenVINOGpuBase::GUID = FGuid((int32)'O', (int32)'V', (int32)'_', (int32)'G');
int32 UNNERuntimeOpenVINOGpuBase::Version = 0x00000001;

FModelInstanceOpenVINOGpu::~FModelInstanceOpenVINOGpu()
{
	if (CompiledModel)
	{
		ov_compiled_model_free(CompiledModel);
	}
}

bool FModelInstanceOpenVINOGpu::Init(TSharedRef<UE::NNE::FSharedModelData> ModelData)
{
	FString DeviceName;

	int32 NumGPUs = 0;
	if (HasMultiGpu(NumGPUs))
	{
		const UNNERuntimeOpenVINOGpuSettings* Settings = GetDefault<UNNERuntimeOpenVINOGpuSettings>();
		if (Settings && Settings->MultiGpuPreference >= 0)
		{
			// Clamp GPU selection to avoid attempting to get a GPU that doesn't exist.
			int32 GPUSelect = FMath::Min(NumGPUs, Settings->MultiGpuPreference);
			DeviceName = FString::Format(TEXT("GPU.{0}"), { GPUSelect });
		}
		else
		{
			// Fallback to iGPU if no GPU preference in a multi-gpu setup.
			// If no iGPU is present, this will select the first available dGPU.
			DeviceName = TEXT("GPU.0");
		}
	}
	else
	{
		DeviceName = TEXT("GPU");
	}

	if (!InitModelInstance(ModelData, CompiledModel, DeviceName))
	{
		return false;
	}

	bool bResult = true;

	if (!InitModelTensorDescs(InputSymbolicTensors, OutputSymbolicTensors, CompiledModel))
	{
		bResult = false;
		ov_compiled_model_free(CompiledModel);
		CompiledModel = nullptr;
	}

	return bResult;
}

TConstArrayView<UE::NNE::FTensorDesc> FModelInstanceOpenVINOGpu::GetInputTensorDescs() const
{
	return InputSymbolicTensors;
}

TConstArrayView<UE::NNE::FTensorDesc> FModelInstanceOpenVINOGpu::GetOutputTensorDescs() const
{
	return OutputSymbolicTensors;
}

TConstArrayView<UE::NNE::FTensorShape> FModelInstanceOpenVINOGpu::GetInputTensorShapes() const
{
	return InputTensorShapes;
}

TConstArrayView<UE::NNE::FTensorShape> FModelInstanceOpenVINOGpu::GetOutputTensorShapes() const
{
	return OutputTensorShapes;
}

UE::NNE::EResultStatus FModelInstanceOpenVINOGpu::SetInputTensorShapes(TConstArrayView<UE::NNE::FTensorShape> InInputShapes)
{
	if (!CompiledModel)
	{
		UE_LOG(LogNNERuntimeOpenVINO, Error, TEXT("Invalid compiled model."));
		return UE::NNE::EResultStatus::Fail;
	}

	if (InInputShapes.Num() != InputSymbolicTensors.Num())
	{
		UE_LOG(LogNNERuntimeOpenVINO, Error, TEXT("Input shape sizes don't match."));
		return UE::NNE::EResultStatus::Fail;
	}

	for (int32 i = 0; i < InInputShapes.Num(); ++i)
	{
		const UE::NNE::FTensorDesc SymbolicDesc = InputSymbolicTensors[i];
		if (!InInputShapes[i].IsCompatibleWith(SymbolicDesc.GetShape()))
		{
			UE_LOG(LogNNERuntimeOpenVINO, Error, TEXT("Input shape tensor [%s] doesn't match input tensor [%d]."), *SymbolicDesc.GetName(), i);
			return UE::NNE::EResultStatus::Fail;
		}
	}

	InputTensorShapes = InInputShapes;
	return UE::NNE::EResultStatus::Ok;
}

UE::NNE::EResultStatus FModelInstanceOpenVINOGpu::RunSync(TConstArrayView<UE::NNE::FTensorBindingCPU> InInputTensors, TConstArrayView<UE::NNE::FTensorBindingCPU> InOutputTensors)
{
	return ModelInfer(InInputTensors, InOutputTensors, CompiledModel);
}

FModelOpenVINOGpu::FModelOpenVINOGpu(TSharedRef<UE::NNE::FSharedModelData> InModelData)
	: ModelData(InModelData)
{
}

TSharedPtr<UE::NNE::IModelInstanceGPU> FModelOpenVINOGpu::CreateModelInstanceGPU()
{
	TSharedPtr<FModelInstanceOpenVINOGpu> ModelInstance = MakeShared<FModelInstanceOpenVINOGpu>();
	if (!ModelInstance->Init(ModelData))
	{
		UE_LOG(LogNNERuntimeOpenVINO, Error, TEXT("Failed to initialize the model instance."));
		return {};
	}

	return ModelInstance;
}

FString UNNERuntimeOpenVINOGpuBase::GetRuntimeName() const
{
	return TEXT("NNERuntimeOpenVINOGpu");
}

INNERuntime::ECanCreateModelDataStatus UNNERuntimeOpenVINOGpuBase::CanCreateModelData(const FString& FileType, TConstArrayView64<uint8> FileData, const TMap<FString, TConstArrayView64<uint8>>& AdditionalFileData, const FGuid& FileId, const ITargetPlatform* TargetPlatform) const
{
	return (!FileData.IsEmpty() && IsFileSupported(FileType)) ? ECanCreateModelDataStatus::Ok : ECanCreateModelDataStatus::FailFileIdNotSupported;
}

TSharedPtr<UE::NNE::FSharedModelData> UNNERuntimeOpenVINOGpuBase::CreateModelData(const FString& FileType, TConstArrayView64<uint8> FileData, const TMap<FString, TConstArrayView64<uint8>>& AdditionalFileData, const FGuid& FileId, const ITargetPlatform* TargetPlatform)
{
	if (CanCreateModelData(FileType, FileData, AdditionalFileData, FileId, TargetPlatform) != ECanCreateModelDataStatus::Ok)
	{
		UE_LOG(LogNNERuntimeOpenVINO, Error, TEXT("Cannot create the GPU model data with id %s (Filetype: %s)"), *FileId.ToString(EGuidFormats::Digits).ToLower(), *FileType);
		return {};
	}

	bool bHasWeights = FileType.Compare(TEXT("xml"), ESearchCase::IgnoreCase) == 0;

	TArray64<uint8> WrappedFileData;
	FMemoryWriter64 MemoryWriter(WrappedFileData);
	MemoryWriter << bHasWeights;
	MemoryWriter.Serialize((void*)FileData.GetData(), FileData.NumBytes());

	FSharedBuffer SharedBuffer(FSharedBuffer::Clone(WrappedFileData.GetData(), WrappedFileData.NumBytes()));
	
	TSharedPtr<UE::NNE::FSharedModelData> SharedData(MakeShared<UE::NNE::FSharedModelData>(SharedBuffer, 0));
	return SharedData;
}

FString UNNERuntimeOpenVINOGpuBase::GetModelDataIdentifier(const FString& FileType, TConstArrayView64<uint8> FileData, const TMap<FString, TConstArrayView64<uint8>>& AdditionalFileData, const FGuid& FileId, const ITargetPlatform* TargetPlatform) const
{
	return FileId.ToString(EGuidFormats::Digits) + "-" + UNNERuntimeOpenVINOGpu::GUID.ToString(EGuidFormats::Digits) + "-" + FString::FromInt(UNNERuntimeOpenVINOGpu::Version);
}

INNERuntimeGPU::ECanCreateModelGPUStatus UNNERuntimeOpenVINOGpu::CanCreateModelGPU(const TObjectPtr<UNNEModelData> ModelData) const
{
	check(ModelData != nullptr);

	const TSharedPtr<UE::NNE::FSharedModelData> SharedData = ModelData->GetModelData(GetRuntimeName());

	if (!SharedData.IsValid())
	{
		return ECanCreateModelGPUStatus::Fail;
	}

	TConstArrayView64<uint8> Data = SharedData->GetView();

	if (Data.Num() == 0)
	{
		return ECanCreateModelGPUStatus::Fail;
	}

	return ECanCreateModelGPUStatus::Ok;
}

TSharedPtr<UE::NNE::IModelGPU> UNNERuntimeOpenVINOGpu::CreateModelGPU(const TObjectPtr<UNNEModelData> ModelData)
{
	check(ModelData != nullptr);

	if (CanCreateModelGPU(ModelData) != ECanCreateModelGPUStatus::Ok)
	{
		UE_LOG(LogNNERuntimeOpenVINO, Error, TEXT("Cannot create a GPU model from the model data with id %s"), *ModelData->GetFileId().ToString(EGuidFormats::Digits));
		return TSharedPtr<UE::NNE::IModelGPU>();
	}

	TSharedRef<UE::NNE::FSharedModelData> SharedData = ModelData->GetModelData(GetRuntimeName()).ToSharedRef();
	return MakeShared<FModelOpenVINOGpu>(SharedData);
}

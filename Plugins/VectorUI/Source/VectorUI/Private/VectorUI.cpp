// Copyright Epic Games, Inc. All Rights Reserved.

#include "VectorUI.h"
#include "include/core/SkColor.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FVectorUIModule"

void FVectorUIModule::StartupModule()
{
	FString baseDir = IPluginManager::Get().FindPlugin("VectorUI")->GetBaseDir();

	FString libPath;
#if PLATFORM_WINDOWS
	libPath = FPaths::Combine(*baseDir, TEXT("Binaries/ThirdParty/Skia/Win64/skia.dll"));
#endif
	SkiaHandle = FPaths::FileExists(libPath) ? FPlatformProcess::GetDllHandle(*libPath) : nullptr;

	if (SkiaHandle != nullptr)
	{
		// Simple sanity check to make sure the DLL loaded correctly
		SkScalar hsb[3]{ 200.f, 1.f, 1.f };
		SkColor argb = SkHSVToColor(hsb);

		if (argb == 0xFF00AAFF)
		{
			UE_LOG(LogTemp, Log, TEXT("Successfully loaded skia.dll"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Loading of skia.dll produced unexpected result"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load skia.dll"));
	}
}

void FVectorUIModule::ShutdownModule()
{
	if (SkiaHandle) {
		FPlatformProcess::FreeDllHandle(SkiaHandle);
		SkiaHandle = nullptr;
	}
}
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FVectorUIModule, VectorUI)
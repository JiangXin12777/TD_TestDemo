// Copyright Epic Games, Inc. All Rights Reserved.

#include "TD_CSCameraManagerModule.h"

#define LOCTEXT_NAMESPACE "FTD_CSCameraManagerModule"

void FTD_CSCameraManagerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FTD_CSCameraManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTD_CSCameraManagerModule, TD_CSCameraManager)
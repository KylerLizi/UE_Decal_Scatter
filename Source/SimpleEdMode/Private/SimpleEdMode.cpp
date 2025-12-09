// Copyright Epic Games, Inc. All Rights Reserved.

#include "SimpleEdMode.h"
#include "SimpleEdModeEdMode.h"
#include "EditorModeRegistry.h"

#define LOCTEXT_NAMESPACE "FSimpleEdModeModule"

void FSimpleEdModeModule::StartupModule()
{
	FEditorModeRegistry::Get().RegisterMode<FSimpleEdMode>(
		FSimpleEdMode::EM_SimpleEdModeId,
		LOCTEXT("SimpleEdModeName", "Simple Mode"),
		FSlateIcon(),
		true);
}

void FSimpleEdModeModule::ShutdownModule()
{
	FEditorModeRegistry::Get().UnregisterMode(FSimpleEdMode::EM_SimpleEdModeId);
}

IMPLEMENT_MODULE(FSimpleEdModeModule, SimpleEdMode)

#undef LOCTEXT_NAMESPACE


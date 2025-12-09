// Copyright Epic Games, Inc. All Rights Reserved.

#include "SimpleEdModeEdMode.h"
#include "EditorModeManager.h"

const FEditorModeID FSimpleEdMode::EM_SimpleEdModeId = TEXT("EM_SimpleEdMode");

FSimpleEdMode::FSimpleEdMode()
	: FEdMode()
{
}

void FSimpleEdMode::Enter()
{
	FEdMode::Enter();
}

void FSimpleEdMode::Exit()
{
	FEdMode::Exit();
}

bool FSimpleEdMode::UsesToolkits() const
{
	return false;
}


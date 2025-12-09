// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EdMode.h"

class FSimpleEdMode : public FEdMode
{
public:
	static const FEditorModeID EM_SimpleEdModeId;

	FSimpleEdMode();

	virtual void Enter() override;
	virtual void Exit() override;
	virtual bool UsesToolkits() const override;
};


#pragma once

#include "EdMode.h"

class FSimpleEdModeToolkit;

class FSimpleEdMode : public FEdMode
{
public:
	static const FEditorModeID EM_SimpleEdModeId;

	FSimpleEdMode();
	virtual ~FSimpleEdMode() override;

	// FEdMode
	virtual void Enter() override;
	virtual void Exit() override;
	virtual bool UsesToolkits() const override { return true; }

private:
	TSharedPtr<FSimpleEdModeToolkit> ToolkitInstance;
};
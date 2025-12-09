#include "SimpleEdModeEdMode.h"
#include "SimpleEdModeToolkit.h"
#include "EditorModeManager.h"
#include "Toolkits/ToolkitManager.h"

const FEditorModeID FSimpleEdMode::EM_SimpleEdModeId = TEXT("EM_SimpleEdMode");

FSimpleEdMode::FSimpleEdMode()
{
}

FSimpleEdMode::~FSimpleEdMode()
{
}

void FSimpleEdMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		ToolkitInstance = MakeShareable(new FSimpleEdModeToolkit());
		Toolkit = ToolkitInstance;

		// Use the host provided by the mode owner so the toolkit can dock into the expected UI
		if (Owner)
		{
			Toolkit->Init(Owner->GetToolkitHost());
		}
	}
}

void FSimpleEdMode::Exit()
{
	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	ToolkitInstance.Reset();
	FEdMode::Exit();
}
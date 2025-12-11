#include "Modules/ModuleManager.h"
#include "EditorModeRegistry.h"
#include "SmartToolsEdMode.h"
#include "SmartToolsStyle.h"

#define LOCTEXT_NAMESPACE "FSmartToolsModule"

class FSmartToolsModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		FSmartToolsStyle::Initialize();
		FEditorModeRegistry::Get().RegisterMode<FSmartToolsEdMode>(
			FSmartToolsEdMode::EM_SmartToolsEdModeId,
			LOCTEXT("SmartToolsEdModeName", "SmartTools"),
			FSlateIcon(FSmartToolsStyle::GetStyleSetName(), "SmartTools.ModeIcon"),
			true);
	}

	virtual void ShutdownModule() override
	{
		FSmartToolsStyle::Shutdown();
		FEditorModeRegistry::Get().UnregisterMode(FSmartToolsEdMode::EM_SmartToolsEdModeId);
	}
};

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSmartToolsModule, SmartTools)


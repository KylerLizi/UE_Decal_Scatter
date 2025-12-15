#include "Modules/ModuleManager.h"
#include "EditorModeRegistry.h"
#include "SmartToolsEdMode.h"
#include "SmartToolsStyle.h"
#include "PropertyEditorModule.h"
#include "DecalScatter/DecalScatterVolume.h"
#include "DecalScatter/DecalScatterVolumeDetails.h"

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

		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomClassLayout(
			ADecalScatterVolume::StaticClass()->GetFName(),
			FOnGetDetailCustomizationInstance::CreateStatic(&FDecalScatterVolumeDetails::MakeInstance));
		PropertyModule.NotifyCustomizationModuleChanged();
	}

	virtual void ShutdownModule() override
	{
		if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
		{
			FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
			PropertyModule.UnregisterCustomClassLayout(ADecalScatterVolume::StaticClass()->GetFName());
			PropertyModule.NotifyCustomizationModuleChanged();
		}

		FSmartToolsStyle::Shutdown();
		FEditorModeRegistry::Get().UnregisterMode(FSmartToolsEdMode::EM_SmartToolsEdModeId);
	}
};

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSmartToolsModule, SmartTools)


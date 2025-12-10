#include "SmartToolsStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr<FSlateStyleSet> FSmartToolsStyle::StyleInstance = nullptr;

void FSmartToolsStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FSmartToolsStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

const ISlateStyle& FSmartToolsStyle::Get()
{
	return *StyleInstance;
}

FName FSmartToolsStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("SmartToolsStyle"));
	return StyleSetName;
}

TSharedRef<FSlateStyleSet> FSmartToolsStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	// NOTE: If the plugin folder is renamed to SmartTools, change the string below accordingly.
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("SmartTools")->GetBaseDir() / TEXT("Resources"));

	const FVector2D Icon40x40(40.0f, 40.0f);
	Style->Set("SmartTools.ToolsIcon", new FSlateImageBrush(Style->RootToContentDir(TEXT("Tools"), TEXT(".png")), Icon40x40));
	
	const FVector2D Icon50x50(50.0f, 50.0f);
	Style->Set("SmartTools.PlacementIcon", new FSlateImageBrush(Style->RootToContentDir(TEXT("Placement"), TEXT(".png")), Icon50x50));
	Style->Set("SmartTools.MoveIcon", new FSlateImageBrush(Style->RootToContentDir(TEXT("Move"), TEXT(".png")), Icon50x50));

	const FVector2D Icon30x30(30.0f, 30.0f);
	Style->Set("SmartTools.DecalScatterIcon", new FSlateImageBrush(Style->RootToContentDir(TEXT("DecalScatter"), TEXT(".png")), Icon30x30));

	return Style;
}


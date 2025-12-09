#include "SimpleEdModeStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr<FSlateStyleSet> FSimpleEdModeStyle::StyleInstance = nullptr;

void FSimpleEdModeStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FSimpleEdModeStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

const ISlateStyle& FSimpleEdModeStyle::Get()
{
	return *StyleInstance;
}

FName FSimpleEdModeStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("SimpleEdModeStyle"));
	return StyleSetName;
}

TSharedRef<FSlateStyleSet> FSimpleEdModeStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("SimpleEdMode")->GetBaseDir() / TEXT("Resources"));

	const FVector2D Icon40x40(40.0f, 40.0f);
	Style->Set("SimpleEdMode.PlacementIcon", new FSlateImageBrush(Style->RootToContentDir(TEXT("Placement"), TEXT(".png")), Icon40x40));
    Style->Set("SimpleEdMode.MoveIcon", new FSlateImageBrush(Style->RootToContentDir(TEXT("Move"), TEXT(".png")), Icon40x40));
    Style->Set("SimpleEdMode.ToolsIcon", new FSlateImageBrush(Style->RootToContentDir(TEXT("Tools"), TEXT(".png")), Icon40x40));
	return Style;
}

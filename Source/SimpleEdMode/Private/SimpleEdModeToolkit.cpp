#include "SimpleEdModeToolkit.h"
#include "SimpleEdModeEdMode.h"
#include "EditorModeManager.h"
#include "SimpleEdModeStyle.h"
#include "EditorStyleSet.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "FSimpleEdModeToolkit"

FSimpleEdModeToolkit::FSimpleEdModeToolkit()
{
}

void FSimpleEdModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{
	ToolkitWidget = BuildContent();
	FModeToolkit::Init(InitToolkitHost);

	// Set default category and update the tool panel
	SelectedCategory = FName("Placement");
	UpdateToolsPanel();
}

FName FSimpleEdModeToolkit::GetToolkitFName() const
{
	return FName("SimpleEdMode");
}

FText FSimpleEdModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "SimpleEdMode");
}

class FEdMode* FSimpleEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FSimpleEdMode::EM_SimpleEdModeId);
}

TSharedRef<SWidget> FSimpleEdModeToolkit::BuildContent()
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			BuildCategoryPanel()
		]

		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.Padding(4.0f)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
			.Padding(4.0f)
			[
				SAssignNew(ToolsBox, SVerticalBox)
			]
		];
}

TSharedRef<SWidget> FSimpleEdModeToolkit::BuildCategoryPanel()
{
	return SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
		.Padding(4.0f)
		[
			SNew(SVerticalBox)

			// Placement Category Button
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2.0f)
			[
				SNew(SButton)
				.ToolTipText(LOCTEXT("PlacementCategoryTooltip", "Placement"))
				.OnClicked(this, &FSimpleEdModeToolkit::OnCategorySelected, FName("Placement"))
				[
					SNew(SImage)
					.Image(FSimpleEdModeStyle::Get().GetBrush("SimpleEdMode.PlacementIcon"))
				]
			]

			// Assets Category Button
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2.0f)
			[
				SNew(SButton)
				.ToolTipText(LOCTEXT("AssetsCategoryTooltip", "Assets"))
				.OnClicked(this, &FSimpleEdModeToolkit::OnCategorySelected, FName("Assets"))
				[
					SNew(SImage)
					.Image(FSimpleEdModeStyle::Get().GetBrush("SimpleEdMode.MoveIcon"))
				]
			]
		];
}

FReply FSimpleEdModeToolkit::OnCategorySelected(FName CategoryName)
{
	SelectedCategory = CategoryName;
	UpdateToolsPanel();
	return FReply::Handled();
}

void FSimpleEdModeToolkit::UpdateToolsPanel()
{
	ToolsBox->ClearChildren();

	if (SelectedCategory == FName("Placement"))
	{
		ToolsBox->AddSlot()
			.AutoHeight()
			.Padding(2.0f)
			[
				MakeToolButton(LOCTEXT("DecalScatterTool", "Decal Scatter"), "LevelEditor.Tabs.Placement", FOnClicked::CreateSP(this, &FSimpleEdModeToolkit::OnPlaceDecalScatterVolumeClicked))
			];
	}
	else if (SelectedCategory == FName("Assets"))
	{
		ToolsBox->AddSlot()
			.AutoHeight()
			.Padding(2.0f)
			[
				MakeToolButton(LOCTEXT("AssetTool1", "Create Asset"), "AssetTools.CreateAsset", FOnClicked())
			];
	}
}

FReply FSimpleEdModeToolkit::OnPlaceDecalScatterVolumeClicked()
{
    if (FSimpleEdMode* EdMode = static_cast<FSimpleEdMode*>(GetEditorMode()))
    {
        EdMode->StartPlacingDecalScatterVolume();
    }
	return FReply::Handled();
}

TSharedRef<SWidget> FSimpleEdModeToolkit::MakeToolButton(const FText& ToolName, const FName& IconName, const FOnClicked& OnClickedDelegate)
{
	return SNew(SButton)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
        .OnClicked(OnClickedDelegate)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(FMargin(0, 0, 4, 0))
			[
				SNew(SImage)
				.Image(FEditorStyle::GetBrush(IconName))
			]
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(ToolName)
			]
		];
}

#undef LOCTEXT_NAMESPACE
#undef LOCTEXT_NAMESPACE
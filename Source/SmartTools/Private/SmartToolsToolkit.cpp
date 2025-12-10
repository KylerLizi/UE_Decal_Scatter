#include "SmartToolsToolkit.h"
#include "SmartToolsEdMode.h"
#include "SmartToolsStyle.h"
#include "EditorModeManager.h"
#include "EditorStyleSet.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Layout/SScrollBar.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SSearchBox.h"
#include "IDocumentation.h"

#define LOCTEXT_NAMESPACE "FSmartToolsToolkit"

// ============================================================================
// SSmartToolsTools Implementation
// ============================================================================

void SSmartToolsTools::Construct(const FArguments& InArgs)
{
	ActiveTabName = FName("Placement");
	CurrentSearchText.Empty();

	// Initialize categories and items
	UpdatePlacementCategories();

	// Create tabs panel
	Tabs = SNew(SVerticalBox)
		.Visibility(this, &SSmartToolsTools::GetTabsVisibility);

	// Build tabs
	RefreshTabs();

	// Create scroll bar
	TSharedRef<SScrollBar> ScrollBar = SNew(SScrollBar)
		.Thickness(FVector2D(9.0f, 9.0f));

	// Main layout: Search on top; Left tabs + Right list below
	ChildSlot
	[
		SNew(SVerticalBox)

		// Search box
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(4.0f)
		[
			SAssignNew(SearchBoxPtr, SSearchBox)
			.HintText(LOCTEXT("SearchPlaceables", "Search Tools"))
			.OnTextChanged(this, &SSmartToolsTools::OnSearchChanged)
			.OnTextCommitted(this, &SSmartToolsTools::OnSearchCommitted)
		]

		// Content area
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(0.0f)
		[
			SNew(SHorizontalBox)

			// Left panel - tabs
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				Tabs.ToSharedRef()
			]

			// Right panel - content (list view only)
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SBorder)
				.Padding(FMargin(3))
				.BorderImage(FEditorStyle::GetBrush("ToolPanel.DarkGroupBorder"))
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					[
						SAssignNew(ListView, SListView<TSharedPtr<FPlaceableItem>>)
						.ListItemsSource(&FilteredItems)
						.OnGenerateRow(this, &SSmartToolsTools::OnGenerateWidgetForItem)
						.ExternalScrollbar(ScrollBar)
						.SelectionMode(ESelectionMode::Single)
					]

					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						ScrollBar
					]
				]
			]
		]
	];

	// Initialize list
	RefreshFilteredItems();
}

void SSmartToolsTools::UpdatePlacementCategories()
{
	TabNames.Empty();
	TabNames.Add(FName("Placement"));
	TabNames.Add(FName("Assets"));

	// Initialize all items
	AllItems.Empty();

	// Placement items
	AllItems.Add(MakeShareable(new FPlaceableItem{ FText::FromString("Decal Scatter"), FName("Placement"), "Place a decal scatter volume" }));

	// Assets: empty for now
}

void SSmartToolsTools::RefreshTabs()
{
	if (!Tabs.IsValid())
	{
		return;
	}

	Tabs->ClearChildren();

	for (const FName& TabName : TabNames)
	{
		Tabs->AddSlot()
			.AutoHeight()
			.Padding(4.0f, 2.0f)
			[
				SNew(SButton)
				.ButtonStyle(FEditorStyle::Get(), "FlatButton")
				.ToolTipText(FText::FromName(TabName))
				.OnClicked(FOnClicked::CreateSP(this, &SSmartToolsTools::OnTabClicked, TabName))
				[
					SNew(SImage)
					.Image(GetIconForTab(TabName))
				]
			];
	}
}

const FSlateBrush* SSmartToolsTools::GetIconForTab(FName TabName) const
{
	if (TabName == FName("Placement"))
	{
		return FSmartToolsStyle::Get().GetBrush("SmartTools.PlacementIcon");
	}
	if (TabName == FName("Assets"))
	{
		return FSmartToolsStyle::Get().GetBrush("SmartTools.MoveIcon");
	}
	return FEditorStyle::GetBrush("WhiteBrush");
}

FReply SSmartToolsTools::OnTabClicked(FName TabName)
{
	ActiveTabName = TabName;
	RefreshFilteredItems();
	return FReply::Handled();
}

void SSmartToolsTools::OnSearchChanged(const FText& SearchText)
{
	CurrentSearchText = SearchText.ToString();
	RefreshFilteredItems();
}

void SSmartToolsTools::OnSearchCommitted(const FText& SearchText, ETextCommit::Type /*CommitInfo*/)
{
	CurrentSearchText = SearchText.ToString();
	RefreshFilteredItems();
}

void SSmartToolsTools::RefreshFilteredItems()
{
	FilteredItems.Empty();

	for (const TSharedPtr<FPlaceableItem>& Item : AllItems)
	{
		// Filter by active tab/category
		if (Item->CategoryName != ActiveTabName)
		{
			continue;
		}

		// Filter by search text
		if (!CurrentSearchText.IsEmpty())
		{
			if (!Item->DisplayName.ToString().Contains(CurrentSearchText, ESearchCase::IgnoreCase))
			{
				continue;
			}
		}

		FilteredItems.Add(Item);
	}

	if (ListView.IsValid())
	{
		ListView->RequestListRefresh();
	}
}

EVisibility SSmartToolsTools::GetTabsVisibility() const
{
	return EVisibility::Visible;
}

EVisibility SSmartToolsTools::GetFailedSearchVisibility() const
{
	return FilteredItems.Num() == 0 && !CurrentSearchText.IsEmpty() ? EVisibility::Visible : EVisibility::Collapsed;
}

TSharedRef<ITableRow> SSmartToolsTools::OnGenerateWidgetForItem(TSharedPtr<FPlaceableItem> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	FSlateFontInfo NameFont = FEditorStyle::GetFontStyle("NormalFont");
	// Globally enlarge all right panel item text by 1.5x
	NameFont.Size = FMath::Max(1, FMath::RoundToInt(NameFont.Size * 3 / 2));

	return SNew(STableRow<TSharedPtr<FPlaceableItem>>, OwnerTable)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(4.0f)
			[
				SNew(SImage)
				.Image(GetBrushForItem(Item))
			]

			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			.Padding(4.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(Item->DisplayName)
				.ToolTipText(FText::FromString(Item->ToolTip))
				.Font(NameFont)
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(4.0f)
			[
				SNew(SButton)
				.ContentPadding(0)
				.ButtonStyle(FEditorStyle::Get(), "HelpButton")
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.ToolTip(IDocumentation::Get()->CreateToolTip(
					LOCTEXT("DecalScatterHelpTT", "Open Documentation"),
					nullptr,
					TEXT("Shared/EdMode/SmartTools"),
					TEXT("DecalScatter")
				))
				.OnClicked(FOnClicked::CreateSP(this, &SSmartToolsTools::OnHelpClicked, Item))
				[
					SNew(SImage)
					.Image(FEditorStyle::GetBrush("HelpIcon"))
				]
			]
		];
}

FReply SSmartToolsTools::OnHelpClicked(TSharedPtr<FPlaceableItem> Item)
{
	const FString PreviewLink = TEXT("Shared/EdMode/SmartTools");
	const FString Excerpt = Item.IsValid() ? Item->DisplayName.ToString() : TEXT("Overview");
	IDocumentation::Get()->Open(PreviewLink, Excerpt);
	return FReply::Handled();
}

FReply SSmartToolsTools::OnPlaceDecalScatterVolumeClicked()
{
	if (FSmartToolsEdMode* EdMode = static_cast<FSmartToolsEdMode*>(GLevelEditorModeTools().GetActiveMode(FSmartToolsEdMode::EM_SmartToolsEdModeId)))
	{
		EdMode->StartPlacingDecalScatterVolume();
	}
	return FReply::Handled();
}

const FSlateBrush* SSmartToolsTools::GetBrushForItem(const TSharedPtr<FPlaceableItem>& Item) const
{
	if (!Item.IsValid())
	{
		return FEditorStyle::GetBrush("WhiteBrush");
	}

	if (Item->DisplayName.ToString().Equals(TEXT("Decal Scatter"), ESearchCase::IgnoreCase))
	{
		return FSmartToolsStyle::Get().GetBrush("SmartTools.DecalScatterIcon");
	}

	return FSmartToolsStyle::Get().GetBrush("SmartTools.ToolsIcon");
}

// ============================================================================
// FSmartToolsToolkit Implementation
// ============================================================================

FSmartToolsToolkit::FSmartToolsToolkit() {}

void FSmartToolsToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{
	ToolkitWidget = BuildContent();
	FModeToolkit::Init(InitToolkitHost);
}

FName FSmartToolsToolkit::GetToolkitFName() const
{
	return FName("SmartTools");
}

FText FSmartToolsToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "SmartTools");
}

class FEdMode* FSmartToolsToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FSmartToolsEdMode::EM_SmartToolsEdModeId);
}

TSharedRef<SWidget> FSmartToolsToolkit::BuildContent()
{
	PlacementTools = SNew(SSmartToolsTools);
	return PlacementTools.ToSharedRef();
}

#undef LOCTEXT_NAMESPACE


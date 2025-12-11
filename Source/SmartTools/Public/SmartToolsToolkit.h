#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"
#include "Widgets/SCompoundWidget.h"

class SVerticalBox;
class SHorizontalBox;
class SScrollBar;
class STableViewBase;
class ITableRow;
class SSearchBox;

template <typename ItemType>
class SListView;

// Item structure for the placement list
struct FPlaceableItem
{
	FText DisplayName;
	FName CategoryName;
	FString ToolTip;
};

class SSmartToolsTools : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSmartToolsTools) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	// Search
	void OnSearchChanged(const FText& SearchText);
	void OnSearchCommitted(const FText& SearchText, ETextCommit::Type CommitInfo);
	void RefreshFilteredItems();

	// Tab management
	void UpdateToolsCategories();
	FReply OnTabClicked(FName TabName);
	void RefreshTabs();
	const FSlateBrush* GetIconForTab(FName TabName) const;
	EVisibility GetTabsVisibility() const;
	EVisibility GetFailedSearchVisibility() const;

	// List view callbacks
	TSharedRef<ITableRow> OnGenerateWidgetForItem(TSharedPtr<FPlaceableItem> Item, const TSharedRef<STableViewBase>& OwnerTable);
	const FSlateBrush* GetBrushForItem(const TSharedPtr<FPlaceableItem>& Item) const;

	// Tool callbacks
	FReply OnItemClicked(TSharedPtr<FPlaceableItem> Item);
	FReply OnPlaceDecalScatterVolumeClicked();
	FReply OnDeleteEmptyMeshClicked();
	FReply OnHelpClicked(TSharedPtr<FPlaceableItem> Item);

private:
	// Search
	TSharedPtr<SSearchBox> SearchBoxPtr;
	FString CurrentSearchText;

	// Tab management
	TSharedPtr<SVerticalBox> Tabs;
	FName ActiveTabName;
	TArray<FName> TabNames;

	// Content display
	TSharedPtr<SListView<TSharedPtr<FPlaceableItem>>> ListView;

	// Data
	TArray<TSharedPtr<FPlaceableItem>> AllItems;
	TArray<TSharedPtr<FPlaceableItem>> FilteredItems;
};

class FSmartToolsToolkit : public FModeToolkit
{
public:
	FSmartToolsToolkit();

	// FModeToolkit interface
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<SWidget> GetInlineContent() const override { return ToolkitWidget; }

private:
	TSharedRef<SWidget> BuildContent();

private:
	TSharedPtr<SWidget> ToolkitWidget;
	TSharedPtr<SSmartToolsTools> Tools;
};

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"

class SVerticalBox;

class FSimpleEdModeToolkit : public FModeToolkit
{
public:
	FSimpleEdModeToolkit();

	// FModeToolkit interface
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<SWidget> GetInlineContent() const override { return ToolkitWidget; }

private:
	TSharedRef<SWidget> BuildContent();
	TSharedRef<SWidget> BuildCategoryPanel();
	void UpdateToolsPanel();
	FReply OnCategorySelected(FName CategoryName);
	TSharedRef<SWidget> MakeToolButton(const FText& ToolName, const FName& IconName, const FOnClicked& OnClickedDelegate);
	FReply OnPlaceDecalScatterVolumeClicked();

private:
	TSharedPtr<SWidget> ToolkitWidget;
	TSharedPtr<SVerticalBox> ToolsBox;
	FName SelectedCategory;
};
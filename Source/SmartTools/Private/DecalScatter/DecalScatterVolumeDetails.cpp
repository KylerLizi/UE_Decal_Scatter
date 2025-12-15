#include "DecalScatter/DecalScatterVolumeDetails.h"

#include "DecalScatter/DecalScatterVolume.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "PropertyHandle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

TSharedRef<IDetailCustomization> FDecalScatterVolumeDetails::MakeInstance()
{
	return MakeShared<FDecalScatterVolumeDetails>();
}

void FDecalScatterVolumeDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	DetailBuilder.HideCategory(TEXT("Decal Scatter|Decal Parameters"));
	DetailBuilder.HideCategory(TEXT("Decal Scatter|Scatter Parameters"));

	IDetailCategoryBuilder& MainCategory = DetailBuilder.EditCategory(TEXT("Decal Scatter"), FText::GetEmpty(), ECategoryPriority::Important);

	DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(ADecalScatterVolume, DecalElements));
	DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(ADecalScatterVolume, ScatterCount));
	DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(ADecalScatterVolume, Seed));
	DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(ADecalScatterVolume, bRandomSeed));
	DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(ADecalScatterVolume, bUniformScale));
	DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(ADecalScatterVolume, UniformMinScale));
	DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(ADecalScatterVolume, UniformMaxScale));
	DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(ADecalScatterVolume, NonUniformMinScale));
	DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(ADecalScatterVolume, NonUniformMaxScale));

	TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
	DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);

	MainCategory.AddCustomRow(FText::FromString(TEXT("DecalScatterActions")))
	.WholeRowContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0.0f, 0.0f, 8.0f, 0.0f)
		[
			SNew(SBox)
			.WidthOverride(140.0f)
			.HeightOverride(28.0f)
			[
				SNew(SButton)
				.ToolTipText(FText::FromString(TEXT("Scatter decals inside this volume")))
				.ButtonColorAndOpacity(FLinearColor(0.05f, 0.25f, 0.65f, 1.00f))
				.ContentPadding(FMargin(10.0f, 4.0f))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.OnClicked_Lambda([CustomizedObjects]()
				{
					for (const TWeakObjectPtr<UObject>& Obj : CustomizedObjects)
					{
						if (ADecalScatterVolume* Volume = Cast<ADecalScatterVolume>(Obj.Get()))
						{
							Volume->ScatterDecals();
						}
					}
					return FReply::Handled();
				})
				[
					SNew(STextBlock)
					.ColorAndOpacity(FLinearColor::White)
					.Text(FText::FromString(TEXT("随机摆放贴花")))
				]
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBox)
			.WidthOverride(140.0f)
			.HeightOverride(28.0f)
			[
				SNew(SButton)
				.ToolTipText(FText::FromString(TEXT("Clear previously scattered decals")))
				.ButtonColorAndOpacity(FLinearColor(0.05f, 0.25f, 0.65f, 1.00f))
				.ContentPadding(FMargin(10.0f, 4.0f))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.OnClicked_Lambda([CustomizedObjects]()
				{
					for (const TWeakObjectPtr<UObject>& Obj : CustomizedObjects)
					{
						if (ADecalScatterVolume* Volume = Cast<ADecalScatterVolume>(Obj.Get()))
						{
							Volume->ClearDecals();
						}
					}
					return FReply::Handled();
				})
				[
					SNew(STextBlock)
					.ColorAndOpacity(FLinearColor::White)
					.Text(FText::FromString(TEXT("清除结果")))
				]
			]
		]
	];

	TSharedRef<IPropertyHandle> DecalElementsHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(ADecalScatterVolume, DecalElements));
	TSharedRef<IPropertyHandle> ScatterCountHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(ADecalScatterVolume, ScatterCount));
	TSharedRef<IPropertyHandle> RandomSeedHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(ADecalScatterVolume, bRandomSeed));
	TSharedRef<IPropertyHandle> SeedHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(ADecalScatterVolume, Seed));
	TSharedRef<IPropertyHandle> UniformScaleHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(ADecalScatterVolume, bUniformScale));
	TSharedRef<IPropertyHandle> UniformMinScaleHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(ADecalScatterVolume, UniformMinScale));
	TSharedRef<IPropertyHandle> UniformMaxScaleHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(ADecalScatterVolume, UniformMaxScale));
	TSharedRef<IPropertyHandle> NonUniformMinScaleHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(ADecalScatterVolume, NonUniformMinScale));
	TSharedRef<IPropertyHandle> NonUniformMaxScaleHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(ADecalScatterVolume, NonUniformMaxScale));

	MainCategory.AddProperty(DecalElementsHandle);
	MainCategory.AddProperty(ScatterCountHandle);
	MainCategory.AddProperty(RandomSeedHandle);

	IDetailPropertyRow& SeedRow = MainCategory.AddProperty(SeedHandle);
	SeedRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateLambda([RandomSeedHandle]()
	{
		bool bRandom = true;
		if (RandomSeedHandle->GetValue(bRandom) != FPropertyAccess::Success)
		{
			return EVisibility::Visible;
		}
		return bRandom ? EVisibility::Collapsed : EVisibility::Visible;
	})));

	MainCategory.AddProperty(UniformScaleHandle);

	IDetailPropertyRow& UniformMinScaleRow = MainCategory.AddProperty(UniformMinScaleHandle);
	UniformMinScaleRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateLambda([UniformScaleHandle]()
	{
		bool bUniform = true;
		if (UniformScaleHandle->GetValue(bUniform) != FPropertyAccess::Success)
		{
			return EVisibility::Visible;
		}
		return bUniform ? EVisibility::Visible : EVisibility::Collapsed;
	})));

	IDetailPropertyRow& UniformMaxScaleRow = MainCategory.AddProperty(UniformMaxScaleHandle);
	UniformMaxScaleRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateLambda([UniformScaleHandle]()
	{
		bool bUniform = true;
		if (UniformScaleHandle->GetValue(bUniform) != FPropertyAccess::Success)
		{
			return EVisibility::Visible;
		}
		return bUniform ? EVisibility::Visible : EVisibility::Collapsed;
	})));

	IDetailPropertyRow& NonUniformMinScaleRow = MainCategory.AddProperty(NonUniformMinScaleHandle);
	NonUniformMinScaleRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateLambda([UniformScaleHandle]()
	{
		bool bUniform = true;
		if (UniformScaleHandle->GetValue(bUniform) != FPropertyAccess::Success)
		{
			return EVisibility::Visible;
		}
		return bUniform ? EVisibility::Collapsed : EVisibility::Visible;
	})));

	IDetailPropertyRow& NonUniformMaxScaleRow = MainCategory.AddProperty(NonUniformMaxScaleHandle);
	NonUniformMaxScaleRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateLambda([UniformScaleHandle]()
	{
		bool bUniform = true;
		if (UniformScaleHandle->GetValue(bUniform) != FPropertyAccess::Success)
		{
			return EVisibility::Visible;
		}
		return bUniform ? EVisibility::Collapsed : EVisibility::Visible;
	})));
}

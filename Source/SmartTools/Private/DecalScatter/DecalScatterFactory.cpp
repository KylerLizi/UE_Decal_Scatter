#include "DecalScatter/DecalScatterFactory.h"

#if WITH_EDITOR
#include "DecalScatter/DecalScatterVolume.h"
#include "Components/BrushComponent.h"
#include "Engine/Brush.h"
#include "Model.h"
#include "Builders/CubeBuilder.h"
#include "BSPOps.h"
#include "UObject/Package.h"
#include "UObject/UnrealType.h"
#include "Internationalization/Text.h"

#define LOCTEXT_NAMESPACE "DecalScatterFactory"

UDecalScatterFactory::UDecalScatterFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DisplayName = LOCTEXT("DecalScatterVolumeDisplayName", "Decal Scatter Volume");
	NewActorClass = ADecalScatterVolume::StaticClass();
}

void UDecalScatterFactory::CreateBrushForDecalScatterVolume(AVolume* NewActor, UBrushBuilder* BrushBuilder)
{
	if (!NewActor)
	{
		return;
	}

	NewActor->PreEditChange(nullptr);

	// Use the same object flags as the owner volume
	EObjectFlags ObjectFlags = NewActor->GetFlags() & (RF_Transient | RF_Transactional);

	NewActor->PolyFlags = 0;
	NewActor->Brush = NewObject<UModel>(NewActor, NAME_None, ObjectFlags);
	NewActor->Brush->Initialize(nullptr, true);
	NewActor->Brush->Polys = NewObject<UPolys>(NewActor->Brush, NAME_None, ObjectFlags);
	NewActor->GetBrushComponent()->Brush = NewActor->Brush;

	if (BrushBuilder)
	{
#if WITH_EDITORONLY_DATA
		NewActor->BrushBuilder = DuplicateObject<UBrushBuilder>(BrushBuilder, NewActor);
#endif
		BrushBuilder->Build(NewActor->GetWorld(), NewActor);
	}

	FBSPOps::csgPrepMovingBrush(NewActor);

	// Clear materials to avoid invisible texture deps on volumes
	if (NewActor->Brush && NewActor->Brush->Polys)
	{
		for (int32 PolyIdx = 0; PolyIdx < NewActor->Brush->Polys->Element.Num(); ++PolyIdx)
		{
			FPoly& Poly = NewActor->Brush->Polys->Element[PolyIdx];
			Poly.Material = nullptr;
		}
	}

	NewActor->PostEditChange();
}

void UDecalScatterFactory::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	Super::PostSpawnActor(Asset, NewActor);

	AVolume* VolumeActor = Cast<AVolume>(NewActor);
	if (!VolumeActor)
	{
		return;
	}

	// Default to a cube volume 200x200x200
	UCubeBuilder* Builder = NewObject<UCubeBuilder>();
	Builder->X = 200.0f;
	Builder->Y = 200.0f;
	Builder->Z = 200.0f;
	Builder->WallThickness = 10.0f;
	Builder->Hollow = false;
	Builder->Tessellated = false;

	CreateBrushForDecalScatterVolume(VolumeActor, Builder);
}

#undef LOCTEXT_NAMESPACE
#endif // WITH_EDITOR


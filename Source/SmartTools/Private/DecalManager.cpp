#include "DecalManager.h"
#include "DecalScatter/DecalScatterVolume.h"
#include "Engine/World.h"
#include "Engine/Brush.h"
#include "Components/BrushComponent.h"
#include "Builders/CubeBuilder.h"

FDecalManager::FDecalManager()
	: CurrentPlacingVolume(nullptr)
{
}

FDecalManager::~FDecalManager()
{
	ClearAllVolumes();
}

ADecalScatterVolume* FDecalManager::CreatePlacingVolume(UWorld* World)
{
	if (!World)
	{
		return nullptr;
	}

	// Create a new volume actor
	ADecalScatterVolume* NewVolume = World->SpawnActor<ADecalScatterVolume>();
	
	if (NewVolume)
	{
		// Initialize the brush shape for the volume
		InitializeBrushShape(NewVolume);
		
		CurrentPlacingVolume = NewVolume;
		ManagedVolumes.Add(NewVolume);
	}

	return NewVolume;
}

void FDecalManager::FinalizePlacement(ADecalScatterVolume* Volume)
{
	if (Volume == CurrentPlacingVolume)
	{
		CurrentPlacingVolume = nullptr;
	}

	// Keep the volume in ManagedVolumes for tracking, but it's no longer being placed
	// The volume will remain in the world and is no longer managed by this manager
}

ADecalScatterVolume* FDecalManager::GetCurrentPlacingVolume() const
{
	return CurrentPlacingVolume;
}

void FDecalManager::ClearAllVolumes()
{
	// Stop tracking, but DO NOT destroy volumes; their lifecycle is owned by the level
	ManagedVolumes.Empty();
	CurrentPlacingVolume = nullptr;
}

int32 FDecalManager::GetManagedVolumeCount() const
{
	return ManagedVolumes.Num();
}

void FDecalManager::InitializeBrushShape(ADecalScatterVolume* Volume)
{
	if (!Volume)
	{
		return;
	}

	UBrushComponent* BrushComponent = Volume->GetBrushComponent();
	if (!BrushComponent)
	{
		return;
	}

#if WITH_EDITOR
	// Use UCubeBuilder (Editor-only) to generate a box brush like the UI "Brush Settings"
	UCubeBuilder* CubeBuilder = NewObject<UCubeBuilder>();
	if (CubeBuilder)
	{
		CubeBuilder->X = 200.0f;
		CubeBuilder->Y = 200.0f;
		CubeBuilder->Z = 200.0f;
		CubeBuilder->WallThickness = 10.0f;
		CubeBuilder->Hollow = false;
		CubeBuilder->Tessellated = false;

		CubeBuilder->Build(Volume->GetWorld(), Volume);
	}

	// Refresh rendering/collision after brush rebuild
	BrushComponent->RecreatePhysicsState();
	BrushComponent->MarkRenderStateDirty();
	BrushComponent->InvalidateLightingCache();
#endif
}


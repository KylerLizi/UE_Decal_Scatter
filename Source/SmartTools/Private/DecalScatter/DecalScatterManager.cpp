#include "DecalScatter/DecalScatterManager.h"
#include "DecalScatter/DecalScatterVolume.h"
#include "Engine/World.h"
#include "Engine/Brush.h"
#include "Components/BrushComponent.h"
#include "Builders/CubeBuilder.h"
#include "DecalScatter/DecalScatterFactory.h"

FDecalScatterManager::FDecalScatterManager()
	: CurrentPlacingVolume(nullptr)
{
}

FDecalScatterManager::~FDecalScatterManager()
{
	ClearAllVolumes();
}

ADecalScatterVolume* FDecalScatterManager::CreatePlacingVolume(UWorld* World)
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

void FDecalScatterManager::FinalizePlacement(ADecalScatterVolume* Volume)
{
	if (Volume == CurrentPlacingVolume)
	{
		CurrentPlacingVolume = nullptr;
	}

	// Keep the volume in ManagedVolumes for tracking, but it's no longer being placed
	// The volume will remain in the world and is no longer managed by this manager
}

ADecalScatterVolume* FDecalScatterManager::GetCurrentPlacingVolume() const
{
	return CurrentPlacingVolume;
}

void FDecalScatterManager::ClearAllVolumes()
{
	// Stop tracking, but DO NOT destroy volumes; their lifecycle is owned by the level
	ManagedVolumes.Empty();
	CurrentPlacingVolume = nullptr;
}

int32 FDecalScatterManager::GetManagedVolumeCount() const
{
	return ManagedVolumes.Num();
}

void FDecalScatterManager::InitializeBrushShape(ADecalScatterVolume* Volume)
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
	// Build brush using our ActorFactory helper (engine-like path)
	UCubeBuilder* CubeBuilder = NewObject<UCubeBuilder>();
	if (CubeBuilder)
	{
		CubeBuilder->X = 500.0f;
		CubeBuilder->Y = 500.0f;
		CubeBuilder->Z = 200.0f;
		CubeBuilder->WallThickness = 10.0f;
		CubeBuilder->Hollow = false;
		CubeBuilder->Tessellated = false;

		UDecalScatterFactory::CreateBrushForDecalScatterVolume(Volume, CubeBuilder);
	}

	// Refresh rendering/collision after brush rebuild
	BrushComponent->RecreatePhysicsState();
	BrushComponent->MarkRenderStateDirty();
	BrushComponent->InvalidateLightingCache();
#endif
}


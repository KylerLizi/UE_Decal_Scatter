#include "DecalManager.h"
#include "DecalScatter/DecalScatterVolume.h"
#include "Engine/World.h"

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
	// Destroy all managed volumes
	for (TWeakObjectPtr<ADecalScatterVolume>& VolumePtr : ManagedVolumes)
	{
		if (VolumePtr.IsValid())
		{
			VolumePtr->Destroy();
		}
	}

	ManagedVolumes.Empty();
	CurrentPlacingVolume = nullptr;
}

int32 FDecalManager::GetManagedVolumeCount() const
{
	return ManagedVolumes.Num();
}


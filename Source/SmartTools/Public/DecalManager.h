#pragma once

#include "CoreMinimal.h"

class ADecalScatterVolume;
class UWorld;

/**
 * Manages the creation and lifecycle of DecalScatterVolume actors during placement mode.
 * This manager handles multiple volumes and doesn't manage their lifecycle after placement.
 */
class FDecalManager
{
public:
	FDecalManager();
	~FDecalManager();

	/**
	 * Creates a new DecalScatterVolume actor for placement
	 * @param World The world to spawn the actor in
	 * @return Pointer to the newly created actor, or nullptr if creation failed
	 */
	ADecalScatterVolume* CreatePlacingVolume(UWorld* World);

	/**
	 * Finalizes the placement of a volume (removes it from active management)
	 * @param Volume The volume to finalize
	 */
	void FinalizePlacement(ADecalScatterVolume* Volume);

	/**
	 * Gets the current volume being placed
	 * @return Pointer to the current placing volume, or nullptr if none
	 */
	ADecalScatterVolume* GetCurrentPlacingVolume() const;

	/**
	 * Clears all managed volumes (typically called on mode exit)
	 */
	void ClearAllVolumes();

	/**
	 * Gets the number of volumes currently being managed
	 */
	int32 GetManagedVolumeCount() const;

private:
	/**
	 * Initializes the brush shape for a newly created volume
	 * @param Volume The volume to initialize
	 */
	void InitializeBrushShape(ADecalScatterVolume* Volume);

	/** The volume currently being placed */
	ADecalScatterVolume* CurrentPlacingVolume;

	/** List of all volumes created by this manager */
	TArray<TWeakObjectPtr<ADecalScatterVolume>> ManagedVolumes;
};


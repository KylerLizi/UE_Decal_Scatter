#pragma once

#include "CoreMinimal.h"
#include "ActorFactories/ActorFactory.h"
#include "DecalScatterFactory.generated.h"

class AVolume;
class UBrushBuilder;

/**
 * ActorFactory for ADecalScatterVolume that also exposes a helper to build the volume brush
 * similar to CreateBrushForVolumeActor used by built-in volume factories.
 */
UCLASS()
class SMARTTOOLS_API UDecalScatterFactory : public UActorFactory
{
	GENERATED_BODY()
public:
	UDecalScatterFactory(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual void PostSpawnActor(UObject* Asset, AActor* NewActor) override;

	/** Build/initialize the brush for a volume actor using the given BrushBuilder */
	static void CreateBrushForDecalScatterVolume(AVolume* NewActor, UBrushBuilder* BrushBuilder);
#endif
};


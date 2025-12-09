#pragma once

#include "CoreMinimal.h"

class FEditorViewportClient;
class FViewport;
class HHitProxy;
struct FViewportClick;
class ADecalScatterVolume;
class FSimpleEdMode;

class FDecalScatterManager
{
public:
    FDecalScatterManager(FSimpleEdMode* InEdMode);
    ~FDecalScatterManager();

    void Activate();
    void Deactivate();
    bool IsActive() const { return bIsActive; }

    void Tick(FEditorViewportClient* ViewportClient, float DeltaTime);
    bool HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click);

private:
    FSimpleEdMode* EdMode;
    bool bIsActive = false;
    ADecalScatterVolume* PlacingVolumeActor = nullptr;
};

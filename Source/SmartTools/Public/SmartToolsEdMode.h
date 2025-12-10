#pragma once

#include "EdMode.h"
#include "DecalManager.h"
#include "SmartToolsEdMode.generated.h"

class FSmartToolsToolkit;
class ADecalScatterVolume;

UENUM()
enum class EToolMode : uint8
{
	None,
	PlacingDecalScatterVolume
};

class FSmartToolsEdMode : public FEdMode
{
public:
	static const FEditorModeID EM_SmartToolsEdModeId;

	FSmartToolsEdMode();
	virtual ~FSmartToolsEdMode() override;

	// FEdMode interface
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;
	virtual bool HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) override;
	virtual bool UsesToolkits() const override { return true; }
	// End of FEdMode interface

	/** Called from the toolkit to start placing a volume */
	void StartPlacingDecalScatterVolume();

private:
	TSharedPtr<FSmartToolsToolkit> ToolkitInstance;

	EToolMode CurrentToolMode = EToolMode::None;
	FDecalManager DecalManager;
};


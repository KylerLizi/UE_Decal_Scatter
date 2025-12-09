#pragma once

#include "EdMode.h"
#include "DecalManager.h"
#include "SimpleEdModeEdMode.generated.h"

class FSimpleEdModeToolkit;
class ADecalScatterVolume;

UENUM()
enum class EToolMode : uint8
{
	None,
	PlacingDecalScatterVolume
};

class FSimpleEdMode : public FEdMode
{
public:
	static const FEditorModeID EM_SimpleEdModeId;

	FSimpleEdMode();
	virtual ~FSimpleEdMode() override;

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
	TSharedPtr<FSimpleEdModeToolkit> ToolkitInstance;

	EToolMode CurrentToolMode = EToolMode::None;
	FDecalManager DecalManager;
};
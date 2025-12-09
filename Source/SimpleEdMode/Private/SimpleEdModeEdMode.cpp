#include "SimpleEdModeEdMode.h"
#include "SimpleEdModeToolkit.h"
#include "EditorModeManager.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorViewportClient.h"
#include "Engine/World.h"
#include "DecalScatter/DecalScatterVolume.h"

const FEditorModeID FSimpleEdMode::EM_SimpleEdModeId = TEXT("EM_SimpleEdMode");

FSimpleEdMode::FSimpleEdMode()
{
}

FSimpleEdMode::~FSimpleEdMode()
{
}

void FSimpleEdMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		ToolkitInstance = MakeShareable(new FSimpleEdModeToolkit());
		Toolkit = ToolkitInstance;

		if (Owner)
		{
			Toolkit->Init(Owner->GetToolkitHost());
		}
	}
}

void FSimpleEdMode::Exit()
{
    DecalManager.ClearAllVolumes();
    CurrentToolMode = EToolMode::None;

	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	ToolkitInstance.Reset();
	FEdMode::Exit();
}

void FSimpleEdMode::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
    FEdMode::Tick(ViewportClient, DeltaTime);

    if (CurrentToolMode == EToolMode::PlacingDecalScatterVolume)
    {
        ADecalScatterVolume* PlacingVolume = DecalManager.GetCurrentPlacingVolume();
        if (PlacingVolume)
        {
            // Get mouse position
            FViewportCursorLocation MouseLocation = ViewportClient->GetCursorWorldLocationFromMousePos();

            // Line trace from camera to world to get a valid placement location
            FHitResult HitResult;
            ViewportClient->GetWorld()->LineTraceSingleByChannel(
                HitResult,
                MouseLocation.GetOrigin(),
                MouseLocation.GetOrigin() + MouseLocation.GetDirection() * 100000.0f,
                ECC_Visibility
            );

            if (HitResult.bBlockingHit)
            {
                PlacingVolume->SetActorLocation(HitResult.Location);
            }
        }
    }
}

bool FSimpleEdMode::HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click)
{
    if (CurrentToolMode == EToolMode::PlacingDecalScatterVolume && Click.GetKey() == EKeys::LeftMouseButton)
    {
        ADecalScatterVolume* PlacingVolume = DecalManager.GetCurrentPlacingVolume();
        if (PlacingVolume)
        {
            // Finalize placement
            DecalManager.FinalizePlacement(PlacingVolume);
            CurrentToolMode = EToolMode::None;
            return true; // Consume the click
        }
    }

    return FEdMode::HandleClick(InViewportClient, HitProxy, Click);
}

void FSimpleEdMode::StartPlacingDecalScatterVolume()
{
    if (GetWorld())
    {
        CurrentToolMode = EToolMode::PlacingDecalScatterVolume;
        DecalManager.CreatePlacingVolume(GetWorld());
    }
}
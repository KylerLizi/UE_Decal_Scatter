#include "SmartToolsEdMode.h"
#include "SmartToolsToolkit.h"
#include "EditorModeManager.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorViewportClient.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "DecalScatter/DecalScatterVolume.h"
#include "Math/RotationMatrix.h"

const FEditorModeID FSmartToolsEdMode::EM_SmartToolsEdModeId = TEXT("EM_SmartTools");

FSmartToolsEdMode::FSmartToolsEdMode()
{
}

FSmartToolsEdMode::~FSmartToolsEdMode()
{
}

void FSmartToolsEdMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		ToolkitInstance = MakeShareable(new FSmartToolsToolkit());
		Toolkit = ToolkitInstance;

		if (Owner)
		{
			Toolkit->Init(Owner->GetToolkitHost());
		}
	}
}

void FSmartToolsEdMode::Exit()
{
    DecalScatterManager.ClearAllVolumes();
    CurrentToolMode = EToolMode::None;

	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	ToolkitInstance.Reset();
	FEdMode::Exit();
}

void FSmartToolsEdMode::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
    FEdMode::Tick(ViewportClient, DeltaTime);

    if (CurrentToolMode == EToolMode::PlacingDecalScatterVolume)
    {
        ADecalScatterVolume* PlacingVolume = DecalScatterManager.GetCurrentPlacingVolume();
        if (PlacingVolume)
	    {
	        // Get mouse position
	        FViewportCursorLocation MouseLocation = ViewportClient->GetCursorWorldLocationFromMousePos();

	        // Line trace from camera to world to get a valid placement location
	        FHitResult HitResult;
	        const FVector RayStart = MouseLocation.GetOrigin();
	        const FVector RayDir = MouseLocation.GetDirection();
	        const FVector RayEnd = RayStart + RayDir * 100000.0f;

	        FCollisionQueryParams Params(SCENE_QUERY_STAT(DecalScatterPlacement), /*bTraceComplex*/ true);
	        Params.AddIgnoredActor(PlacingVolume);

	        const bool bHit = ViewportClient->GetWorld()->LineTraceSingleByChannel(
	            HitResult,
	            RayStart,
	            RayEnd,
	            ECC_Visibility,
	            Params
	        );

	        if (bHit && HitResult.bBlockingHit)
	        {
	        	// Align volume up axis to surface normal
	        	const FVector Up = HitResult.Normal.GetSafeNormal();
	        	// Derive a stable tangent from the view direction projected onto the hit plane
	        	FVector ViewDir = ViewportClient->GetViewRotation().Vector();
	        	FVector TangentX = (ViewDir - FVector::DotProduct(ViewDir, Up) * Up).GetSafeNormal();
	        	if (TangentX.IsNearlyZero())
	        	{
	        		// Fallback axis if view is parallel to normal
	        		TangentX = FVector::CrossProduct(Up, FVector::RightVector).GetSafeNormal();
	        		if (TangentX.IsNearlyZero())
	        		{
	        			TangentX = FVector::ForwardVector; // final fallback
	        		}
	        	}
	        	const FRotator NewRot = FRotationMatrix::MakeFromZX(Up, TangentX).Rotator();
	        	PlacingVolume->SetActorRotation(NewRot);

	        	// Offset so the bottom of the box sits on the surface
	        	if (const UBrushComponent* BrushComp = PlacingVolume->GetBrushComponent())
	        	{
	        		// Use local bounds so Z extent is half-height in actor space.
	        		const FBoxSphereBounds LocalBounds = BrushComp->CalcBounds(FTransform::Identity);
	        		const float LocalHalfHeight = LocalBounds.BoxExtent.Z * FMath::Abs(BrushComp->GetComponentTransform().GetScale3D().Z);
	        		PlacingVolume->SetActorLocation(HitResult.Location + Up * LocalHalfHeight);
	        	}
	        	else
	        	{
	        		PlacingVolume->SetActorLocation(HitResult.Location);
	        	}
	        }
	        else
	        {
	        	// Fallback: project onto ground plane (Z=0) if nothing hit
	        	if (!FMath::IsNearlyZero(RayDir.Z))
	        	{
	        		const double T = -RayStart.Z / RayDir.Z;
	        		if (T > 0.0)
	        		{
	        			const FVector Pos = RayStart + RayDir * T;
	        			// Keep upright in fallback
	        			PlacingVolume->SetActorRotation(FRotator::ZeroRotator);
	        			if (const UBrushComponent* BrushComp = PlacingVolume->GetBrushComponent())
	        			{
	        				const float ExtentZ = BrushComp->Bounds.BoxExtent.Z;
	        				PlacingVolume->SetActorLocation(Pos + FVector::UpVector * ExtentZ);
	        			}
	        			else
	        			{
	        				PlacingVolume->SetActorLocation(Pos);
	        			}
	        		}
	        	}
	        }
	    }
    }
}

bool FSmartToolsEdMode::HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click)
{
    if (CurrentToolMode == EToolMode::PlacingDecalScatterVolume && Click.GetKey() == EKeys::LeftMouseButton)
    {
        ADecalScatterVolume* PlacingVolume = DecalScatterManager.GetCurrentPlacingVolume();
        if (PlacingVolume)
        {
            // Finalize placement
            DecalScatterManager.FinalizePlacement(PlacingVolume);
            CurrentToolMode = EToolMode::None;
            return true; // Consume the click
        }
    }

    return FEdMode::HandleClick(InViewportClient, HitProxy, Click);
}

void FSmartToolsEdMode::StartPlacingDecalScatterVolume()
{
    if (GetWorld())
    {
        CurrentToolMode = EToolMode::PlacingDecalScatterVolume;
        DecalScatterManager.CreatePlacingVolume(GetWorld());
    }
}


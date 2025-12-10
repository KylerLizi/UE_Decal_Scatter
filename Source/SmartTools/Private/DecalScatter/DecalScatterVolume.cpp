#include "DecalScatter/DecalScatterVolume.h"
#include "DecalScatter/DecalDataAsset.h"
#include "Engine/Brush.h"
#include "Components/DecalComponent.h"
#include "Engine/World.h"
#include "Engine/DecalActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BrushComponent.h"

ADecalScatterVolume::ADecalScatterVolume()
{
    // AVolume defaults to a box brush, so we only need to set the color.
    BrushColor = FColor(255, 255, 0, 255);
}

void ADecalScatterVolume::ScatterDecals()
{
    if (!DecalDataAsset || DecalDataAsset->DecalMaterials.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Decal Data Asset is not set or is empty."));
        return;
    }

    if (ScatterCount <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Scatter Count must be greater than 0."));
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    // Use the random seed to get reproducible results
    FRandomStream RandomStream(RandomSeed);

    FBoxSphereBounds Bounds = GetBrushComponent()->CalcBounds(GetActorTransform());
    FBox BoundingBox = Bounds.GetBox();

    for (int32 i = 0; i < ScatterCount; ++i)
    {
        FVector RandomPointInBox = UKismetMathLibrary::RandomPointInBoundingBox(BoundingBox.Min, BoundingBox.Max);

        FVector TraceStart = RandomPointInBox;
        FVector TraceEnd = RandomPointInBox - FVector(0, 0, BoundingBox.GetSize().Z * 2.0f); // Trace downwards

        FHitResult HitResult;
        FCollisionQueryParams CollisionParams;
        CollisionParams.bTraceComplex = true; // Use complex collision for more accurate surface detection

        if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, CollisionParams))
        {
            // Get a random material
            int32 RandomMaterialIndex = RandomStream.RandRange(0, DecalDataAsset->DecalMaterials.Num() - 1);
            UMaterialInterface* DecalMaterial = DecalDataAsset->DecalMaterials[RandomMaterialIndex].LoadSynchronous();

            if (!DecalMaterial)
            {
                continue;
            }

            // Get random rotation and size
            FRotator RandomRotation = FRotator(
                RandomStream.FRandRange(MinRotation.Pitch, MaxRotation.Pitch),
                RandomStream.FRandRange(MinRotation.Yaw, MaxRotation.Yaw),
                RandomStream.FRandRange(MinRotation.Roll, MaxRotation.Roll)
            );

            FVector RandomSize = FVector(
                RandomStream.FRandRange(MinSize.X, MaxSize.X),
                RandomStream.FRandRange(MinSize.Y, MaxSize.Y),
                RandomStream.FRandRange(MinSize.Z, MaxSize.Z)
            );

            // Create rotation from surface normal and add random roll
            FRotator FinalRotation = UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal);
            FinalRotation.Roll += RandomRotation.Roll;
            FinalRotation.Pitch += RandomRotation.Pitch;
            FinalRotation.Yaw += RandomRotation.Yaw;

            ADecalActor* NewDecal = World->SpawnActor<ADecalActor>(HitResult.Location, FinalRotation);
            if (NewDecal)
            {
                NewDecal->SetDecalMaterial(DecalMaterial);
                NewDecal->GetDecal()->DecalSize = RandomSize;
                NewDecal->SetActorScale3D(FVector(1.0f)); // Ensure scale is applied via DecalSize
            }
        }
    }
}


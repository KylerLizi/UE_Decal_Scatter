#include "DecalScatter/DecalScatterVolume.h"
#include "Engine/Brush.h"
#include "Components/DecalComponent.h"
#include "Engine/World.h"
#include "Engine/DecalActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BrushComponent.h"
#include "DrawDebugHelpers.h"

namespace
{
	static FIntVector ToGridKey(const FVector& P, float CellSize)
	{
		return FIntVector(
			FMath::FloorToInt(P.X / CellSize),
			FMath::FloorToInt(P.Y / CellSize),
			FMath::FloorToInt(P.Z / CellSize));
	}

	static bool HasNearbyPointWithinRadius(
		const TMap<FIntVector, TArray<FVector>>& Grid,
		float CellSize,
		const FVector& P,
		float Radius)
	{
		const float RadiusSq = Radius * Radius;
		const FIntVector Key = ToGridKey(P, CellSize);

		for (int32 Dx = -1; Dx <= 1; ++Dx)
		{
			for (int32 Dy = -1; Dy <= 1; ++Dy)
			{
				for (int32 Dz = -1; Dz <= 1; ++Dz)
				{
					const FIntVector Nk(Key.X + Dx, Key.Y + Dy, Key.Z + Dz);
					if (const TArray<FVector>* Bucket = Grid.Find(Nk))
					{
						for (const FVector& Q : *Bucket)
						{
							if (FVector::DistSquared(P, Q) < RadiusSq)
							{
								return true;
							}
						}
					}
				}
			}
		}

		return false;
	}
}

ADecalScatterVolume::ADecalScatterVolume()
{
    // AVolume defaults to a box brush, so we only need to set the color.
    BrushColor = FColor(255, 255, 0, 255);
}

// --- Helpers ---

void ADecalScatterVolume::GetNormalizedUniformScaleRange(float& OutMin, float& OutMax) const
{
    OutMin = FMath::Max(0.f, FMath::Min(UniformMinScale, UniformMaxScale));
    OutMax = FMath::Max(OutMin, FMath::Max(UniformMinScale, UniformMaxScale));
}

void ADecalScatterVolume::GetNormalizedNonUniformScaleRange(FVector& OutMin, FVector& OutMax) const
{
    const FVector RawMin = NonUniformMinScale;
    const FVector RawMax = NonUniformMaxScale;

    OutMin.X = FMath::Max(0.f, FMath::Min(RawMin.X, RawMax.X));
    OutMin.Y = FMath::Max(0.f, FMath::Min(RawMin.Y, RawMax.Y));
    OutMin.Z = FMath::Max(0.f, FMath::Min(RawMin.Z, RawMax.Z));

    OutMax.X = FMath::Max(OutMin.X, FMath::Max(RawMin.X, RawMax.X));
    OutMax.Y = FMath::Max(OutMin.Y, FMath::Max(RawMin.Y, RawMax.Y));
    OutMax.Z = FMath::Max(OutMin.Z, FMath::Max(RawMin.Z, RawMax.Z));
}

const FDecalScatterElement* ADecalScatterVolume::SelectRandomElement(FRandomStream& RandomStream, float TotalWeight) const
{
    if (DecalElements.Num() == 0 || TotalWeight <= 0.f)
    {
        return nullptr;
    }

    float RandomValue = RandomStream.FRand() * TotalWeight;
    float AccumulatedWeight = 0.0f;
    for (const FDecalScatterElement& Element : DecalElements)
    {
        AccumulatedWeight += FMath::Max(Element.Weight, 0.0f);
        if (RandomValue <= AccumulatedWeight)
        {
            return &Element;
        }
    }
    return &DecalElements[0];
}

FVector ADecalScatterVolume::ComputeRandomActorScale(FRandomStream& RandomStream) const
{
    if (bUniformScale)
    {
        float SMin = 0.f, SMax = 0.f;
        GetNormalizedUniformScaleRange(SMin, SMax);
        const float S = RandomStream.FRandRange(SMin, SMax);
        return FVector(S, S, S);
    }

    FVector SMin(1.f, 1.f, 1.f);
    FVector SMax(1.f, 1.f, 1.f);
    GetNormalizedNonUniformScaleRange(SMin, SMax);

    const float Sx = RandomStream.FRandRange(SMin.X, SMax.X);
    const float Sy = RandomStream.FRandRange(SMin.Y, SMax.Y);
    const float Sz = RandomStream.FRandRange(SMin.Z, SMax.Z);
    return FVector(Sx, Sy, Sz);
}

FRotator ADecalScatterVolume::ComputeRandomActorRotation(FRandomStream& RandomStream) const
{
    const float Yaw = RandomStream.FRandRange(0.f, 360.f);
    const FQuat BaseRot = FQuat(FRotator(-90.f, 0.f, 0.f));
    const FQuat YawQuat = FQuat(FRotator(0.f, Yaw, 0.f));
    return (YawQuat * BaseRot).Rotator();
}

ADecalActor* ADecalScatterVolume::SpawnDecalActor(UWorld* World,
                                 UMaterialInterface* DecalMaterial,
                                 const FString& DecalBaseName,
                                 const FVector& Location,
                                 const FRotator& Rotation,
                                 const FVector& ActorScale) const
{
    if (!IsValid(World) || !IsValid(DecalMaterial))
    {
        return nullptr;
    }

	if (Location.ContainsNaN()
		|| ActorScale.ContainsNaN()
		|| !FMath::IsFinite(Location.X) || !FMath::IsFinite(Location.Y) || !FMath::IsFinite(Location.Z)
		|| !FMath::IsFinite(ActorScale.X) || !FMath::IsFinite(ActorScale.Y) || !FMath::IsFinite(ActorScale.Z)
		|| !FMath::IsFinite(Rotation.Pitch) || !FMath::IsFinite(Rotation.Yaw) || !FMath::IsFinite(Rotation.Roll))
	{
		return nullptr;
	}

	const FVector SafeScale(
		FMath::Max(ActorScale.X, KINDA_SMALL_NUMBER),
		FMath::Max(ActorScale.Y, KINDA_SMALL_NUMBER),
		FMath::Max(ActorScale.Z, KINDA_SMALL_NUMBER));

    FActorSpawnParameters SpawnParams;
    if (ULevel* Level = GetLevel())
    {
		SpawnParams.OverrideLevel = Level;
    }
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ADecalActor* NewDecal = World->SpawnActor<ADecalActor>(Location, FRotator::ZeroRotator, SpawnParams);
    if (NewDecal)
    {
        NewDecal->SetActorRotation(Rotation);
        NewDecal->SetDecalMaterial(DecalMaterial);
        // Fixed decal size as requested (X=128, Y=256, Z=256)
		if (UDecalComponent* DecalComp = NewDecal->GetDecal())
		{
			DecalComp->DecalSize = FVector(128.f, 256.f, 256.f);
		}
        NewDecal->SetActorScale3D(SafeScale);
#if WITH_EDITOR
        NewDecal->SetActorLabel(DecalBaseName);
#endif
    }
    return NewDecal;
}

bool ADecalScatterVolume::SampleAndHitWithinVolume(FRandomStream& RandomStream,
                                  UBrushComponent* BrushComp,
                                  const FBox& WorldAABB,
                                  FVector& OutWorldSample,
                                  FHitResult& OutHit) const
{
    if (!BrushComp)
    {
        return false;
    }

    // Build OBB in component-local space and transform to world
    const FTransform BrushXf = BrushComp->GetComponentTransform();
    const FBoxSphereBounds LocalBounds = BrushComp->CalcBounds(FTransform::Identity);
    const FVector C = LocalBounds.Origin;
    const FVector E = LocalBounds.BoxExtent;

    // Sample a local point within OBB
    const FVector LocalSample(
        RandomStream.FRandRange(C.X - E.X, C.X + E.X),
        RandomStream.FRandRange(C.Y - E.Y, C.Y + E.Y),
        RandomStream.FRandRange(C.Z - E.Z, C.Z + E.Z)
    );
    const FVector WorldSample = BrushXf.TransformPosition(LocalSample);

    if (!EncompassesPoint(WorldSample))
    {
        return false;
    }

    // Trace downward to find a surface, only accept hits still inside volume
    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }

    const FVector TraceStart = WorldSample;
    const FVector TraceEnd = WorldSample - FVector(0, 0, WorldAABB.GetSize().Z * 2.0f);

    FCollisionQueryParams CollisionParams;
    CollisionParams.bTraceComplex = true;

    if (!World->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECC_Visibility, CollisionParams))
    {
        return false;
    }

    if (!EncompassesPoint(OutHit.Location))
    {
        return false;
    }

    OutWorldSample = WorldSample;
    return true;
}

// --- End Helpers ---

void ADecalScatterVolume::ScatterDecals()
{
	if (bIsScattering)
	{
		return;
	}
	TGuardValue<bool> ReentrancyGuard(bIsScattering, true);

    // Validation
    if (DecalElements.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Decal Elements array is empty. Please add decal elements to scatter."));
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

    // Reset previous decals
    ClearDecals();

    // Random stream for determinism or per-click randomization
    int32 SeedToUse = Seed;
    if (bRandomSeed)
    {
        SeedToUse = static_cast<int32>(FDateTime::Now().GetTicks() & 0x7fffffff);
        Seed = SeedToUse; // persist the new seed so the UI reflects it
    }
    FRandomStream RandomStream(SeedToUse);

    // Get brush and world AABB once
    UBrushComponent* BrushComp = GetBrushComponent();
    if (!BrushComp)
    {
        return;
    }
    const FBox WorldAABB = BrushComp->CalcBounds(GetActorTransform()).GetBox();

    // Precompute total weight
    float TotalWeight = 0.0f;
    for (const FDecalScatterElement& Element : DecalElements)
    {
        TotalWeight += FMath::Max(Element.Weight, 0.0f);
    }
    if (TotalWeight <= 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("Total weight of decal elements is 0. Please set positive weights."));
        return;
    }

	const float EffectiveMinSpacing = FMath::Max(0.0f, MinDecalSpacing);
	const float GridCellSize = (EffectiveMinSpacing > 0.0f) ? EffectiveMinSpacing : 1.0f;
	TMap<FIntVector, TArray<FVector>> PlacedPointGrid;

    // Scatter loop
    for (int32 i = 0; i < ScatterCount; ++i)
    {
        bool bSpawned = false;
        const int32 MaxAttemptsPerDecal = 64;

        for (int32 Attempt = 0; Attempt < MaxAttemptsPerDecal; ++Attempt)
        {
            FVector WorldSample;
            FHitResult Hit;
            if (!SampleAndHitWithinVolume(RandomStream, BrushComp, WorldAABB, WorldSample, Hit))
            {
                continue; // resample
            }

			if (EffectiveMinSpacing > 0.0f)
			{
				if (HasNearbyPointWithinRadius(PlacedPointGrid, GridCellSize, Hit.Location, EffectiveMinSpacing))
				{
					continue; // too close to an existing decal, try again
				}
			}

            // Debug draw: sample (green) and hit (red)
            //DrawDebugSphere(World, WorldSample, 10.f, 12, FColor::Green, false, 3.0f, 0, 1.25f);
            //DrawDebugSphere(World, Hit.Location, 8.f, 12, FColor::Red, false, 3.0f, 0, 1.25f);

            // Pick a decal element and load material
            const FDecalScatterElement* Selected = SelectRandomElement(RandomStream, TotalWeight);
            if (!Selected)
            {
                continue; // resample
            }
            UMaterialInterface* Mat = Selected->DecalMaterial.LoadSynchronous();
            if (!Mat)
            {
                continue; // resample
            }

            // Rotation/Scale and spawn
            const FVector ActorScale = ComputeRandomActorScale(RandomStream);
            const FRotator ActorRot = ComputeRandomActorRotation(RandomStream);
            const FString DecalBaseName = Mat->GetName();
            if (ADecalActor* NewDecal = SpawnDecalActor(World, Mat, DecalBaseName, Hit.Location, ActorRot, ActorScale))
            {
                SpawnedDecals.Add(NewDecal);

				if (EffectiveMinSpacing > 0.0f)
				{
					const FIntVector Key = ToGridKey(Hit.Location, GridCellSize);
					PlacedPointGrid.FindOrAdd(Key).Add(Hit.Location);
				}

                bSpawned = true;
                break; // next scatter
            }
        }

        if (!bSpawned)
        {
            UE_LOG(LogTemp, Verbose, TEXT("Decal scatter: failed to place decal after attempts (volume=%s)"), *GetName());
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Scattered %d decals. Total spawned decals: %d"), ScatterCount, SpawnedDecals.Num());
}

void ADecalScatterVolume::ClearDecals()
{
    // Remove null pointers and destroy valid actors
    for (int32 i = SpawnedDecals.Num() - 1; i >= 0; --i)
    {
        TWeakObjectPtr<ADecalActor> WeakActor = SpawnedDecals[i];
        if (ADecalActor* DecalActor = WeakActor.Get())
        {
            if (!DecalActor->IsActorBeingDestroyed())
            {
                DecalActor->Destroy();
            }
        }
    }
    SpawnedDecals.Empty();
    UE_LOG(LogTemp, Log, TEXT("Cleared all spawned decals."));
}



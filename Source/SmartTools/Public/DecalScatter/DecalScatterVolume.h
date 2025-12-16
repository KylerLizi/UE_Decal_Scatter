#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "DecalScatterVolume.generated.h"

class ADecalActor;
class UMaterialInterface;
class UBrushComponent;

USTRUCT(BlueprintType)
struct FDecalScatterElement
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
    TSoftObjectPtr<UMaterialInterface> DecalMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal", meta = (DisplayName = "Weight"))
    float Weight = 1.0f;
};

UCLASS()
class SMARTTOOLS_API ADecalScatterVolume : public ATriggerVolume
{
    GENERATED_BODY()

public:
    ADecalScatterVolume();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Decal Elements"))
    TArray<FDecalScatterElement> DecalElements;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ScatterCount = 10;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Min Decal Spacing", ClampMin = "0.0"))
    float MinDecalSpacing = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Seed = 123;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Random Seed"))
    bool bRandomSeed = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Uniform Scale"))
    bool bUniformScale = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Scale Min", ClampMin = "0.0"))
    float UniformMinScale = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Scale Max", ClampMin = "0.0"))
    float UniformMaxScale = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Scale Min"))
    FVector NonUniformMinScale = FVector(1.f, 1.f, 1.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Scale Max"))
    FVector NonUniformMaxScale = FVector(1.f, 1.f, 1.f);

private:
    UPROPERTY()
    TArray<TWeakObjectPtr<ADecalActor>> SpawnedDecals;

    bool bIsScattering = false;

    // Helpers
    bool SampleAndHitWithinVolume(FRandomStream& RandomStream,
                                  class UBrushComponent* BrushComp,
                                  const FBox& WorldAABB,
                                  FVector& OutWorldSample,
                                  FHitResult& OutHit) const;

    const FDecalScatterElement* SelectRandomElement(FRandomStream& RandomStream, float TotalWeight) const;

    FVector ComputeRandomActorScale(FRandomStream& RandomStream) const;
    FRotator ComputeRandomActorRotation(FRandomStream& RandomStream) const;

    ADecalActor* SpawnDecalActor(UWorld* World,
                                 UMaterialInterface* DecalMaterial,
                                 const FString& DecalBaseName,
                                 const FVector& Location,
                                 const FRotator& Rotation,
                                 const FVector& ActorScale) const;

    void GetNormalizedUniformScaleRange(float& OutMin, float& OutMax) const;
    void GetNormalizedNonUniformScaleRange(FVector& OutMin, FVector& OutMax) const;

public:
    void ScatterDecals();

    void ClearDecals();
};

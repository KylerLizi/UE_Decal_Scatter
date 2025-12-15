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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00 Decal Scatter|Decal Parameters", meta = (DisplayName = "Decal Elements"))
    TArray<FDecalScatterElement> DecalElements;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "00 Decal Scatter|Scatter Parameters")
    int32 ScatterCount = 10;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "00 Decal Scatter|Scatter Parameters")
    int32 Seed = 123;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "00 Decal Scatter|Scatter Parameters", meta = (DisplayName = "Random Seed"))
    bool bRandomSeed = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "00 Decal Scatter|Decal Parameters", meta = (DisplayName = "Base Scale"))
    FVector BaseScale = FVector(1.f, 1.f, 1.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "00 Decal Scatter|Decal Parameters", meta = (DisplayName = "Uniform Scale"))
    bool bUniformScale = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "00 Decal Scatter|Decal Parameters", meta = (DisplayName = "Scale Min", ClampMin = "0.0"))
    float MinScale = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "00 Decal Scatter|Decal Parameters", meta = (DisplayName = "Scale Max", ClampMin = "0.0"))
    float MaxScale = 1.f;

private:
    UPROPERTY()
    TArray<TWeakObjectPtr<ADecalActor>> SpawnedDecals;

    // Helpers
    bool SampleAndHitWithinVolume(FRandomStream& RandomStream,
                                  class UBrushComponent* BrushComp,
                                  const FBox& WorldAABB,
                                  FVector& OutWorldSample,
                                  FHitResult& OutHit) const;

    const FDecalScatterElement* SelectRandomElement(FRandomStream& RandomStream, float TotalWeight) const;

    FVector ComputeRandomActorScale(FRandomStream& RandomStream, float SMin, float SMax) const;
    FRotator ComputeRandomActorRotation(FRandomStream& RandomStream) const;

    ADecalActor* SpawnDecalActor(UWorld* World,
                                 UMaterialInterface* DecalMaterial,
                                 const FString& DecalBaseName,
                                 const FVector& Location,
                                 const FRotator& Rotation,
                                 const FVector& ActorScale) const;

    void GetNormalizedScaleRange(float& OutMin, float& OutMax) const;

public:
    UFUNCTION(CallInEditor, Category = "00 Decal Scatter")
    void ScatterDecals();

    UFUNCTION(CallInEditor, Category = "00 Decal Scatter")
    void ClearDecals();
};


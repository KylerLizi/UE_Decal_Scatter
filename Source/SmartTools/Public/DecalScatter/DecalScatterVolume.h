#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "DecalScatterVolume.generated.h"

class UDecalDataAsset;

UCLASS()
class SMARTTOOLS_API ADecalScatterVolume : public AVolume
{
    GENERATED_BODY()

public:
    ADecalScatterVolume();

    //~ Begin Scatter Parameters
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Decal Scatter|Scatter Parameters")
    int32 ScatterCount = 100;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Decal Scatter|Scatter Parameters")
    int32 RandomSeed = 1234;
    //~ End Scatter Parameters


    //~ Begin Decal Parameters
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Decal Scatter|Decal Parameters", meta = (DisplayName = "Size Min"))
    FVector MinSize = FVector(50.f, 50.f, 50.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Decal Scatter|Decal Parameters", meta = (DisplayName = "Size Max"))
    FVector MaxSize = FVector(100.f, 100.f, 100.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Decal Scatter|Decal Parameters", meta = (DisplayName = "Rotation Min (Degrees)"))
    FRotator MinRotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Decal Scatter|Decal Parameters", meta = (DisplayName = "Rotation Max (Degrees)"))
    FRotator MaxRotation = FRotator(360.f, 360.f, 360.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Decal Scatter|Decal Parameters")
    TSoftObjectPtr<UDecalDataAsset> DecalDataAsset;
    //~ End Decal Parameters

    UFUNCTION(CallInEditor, Category = "Decal Scatter")
    void ScatterDecals();
};


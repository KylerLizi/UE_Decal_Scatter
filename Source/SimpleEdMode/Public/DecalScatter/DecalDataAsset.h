#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DecalDataAsset.generated.h"

UCLASS(BlueprintType)
class SIMPLEEDMODE_API UDecalDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Decals")
	TArray<TSoftObjectPtr<UMaterialInterface>> DecalMaterials;
};

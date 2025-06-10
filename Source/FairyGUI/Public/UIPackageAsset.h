#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UIPackageAsset.generated.h"

UCLASS()
class FAIRYGUI_API UUIPackageAsset final : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FairyGUI")
    FString Name;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FairyGUI")
    FString ID;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FairyGUI")
    TArray<struct FUIPackageDependency> Dependencies;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FairyGUI")
    TMap<FString, TSoftObjectPtr<UObject>> Resources;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FairyGUI")
    TArray<uint8> Data;

#if WITH_EDITORONLY_DATA
    UPROPERTY(Instanced)
    TObjectPtr<UAssetImportData> AssetImportData;

    virtual void GetAssetRegistryTags(FAssetRegistryTagsContext Context) const override;
#endif
};
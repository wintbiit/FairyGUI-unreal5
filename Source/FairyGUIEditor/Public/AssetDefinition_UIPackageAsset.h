// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetDefinitionDefault.h"
#include "AssetDefinition_UIPackageAsset.generated.h"

/**
 * 
 */
UCLASS()
class FAIRYGUIEDITOR_API UAssetDefinition_UIPackageAsset final : public UAssetDefinitionDefault
{
	GENERATED_BODY()
public:
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
	virtual FText GetAssetDisplayName() const override { return FText::FromString(TEXT("UIPackage")); }
	virtual FLinearColor GetAssetColor() const override { return FLinearColor(0.2f, 0.6f, 0.8f); }
	virtual bool CanImport() const override { return true; }
	virtual FAssetSupportResponse CanRename(const FAssetData& InAsset) const override { return FAssetSupportResponse::NotSupported(); }
	virtual FAssetSupportResponse CanDuplicate(const FAssetData& InAsset) const override { return FAssetSupportResponse::NotSupported(); }
};

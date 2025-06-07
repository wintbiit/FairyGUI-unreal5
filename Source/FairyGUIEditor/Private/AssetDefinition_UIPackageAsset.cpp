// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetDefinition_UIPackageAsset.h"

#include "UIPackageAsset.h"

TSoftClassPtr<UObject> UAssetDefinition_UIPackageAsset::GetAssetClass() const
{
	return UUIPackageAsset::StaticClass();
}

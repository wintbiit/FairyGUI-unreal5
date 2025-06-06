// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetDefinition_UIPackage.h"

#include "UI/UIPackage.h"

TSoftClassPtr<UObject> UAssetDefinition_UIPackage::GetAssetClass() const
{
	return UUIPackage::StaticClass();
}
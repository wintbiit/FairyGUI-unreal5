// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorReimportHandler.h"
#include "AssetTypeActions/AssetTypeActions_DataAsset.h"
#include "Factories/Factory.h"
#include "UIPackageFactory.generated.h"

/**
 * 
 */
UCLASS()
class FAIRYGUIEDITOR_API UUIPackageFactory : public UFactory, public FReimportHandler
{
	GENERATED_BODY()

public:
	UUIPackageFactory();

	virtual UObject* FactoryCreateBinary(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn) override;
	// virtual bool FactoryCanImport(const FString& Filename) override;
	//
	virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
	virtual EReimportResult::Type Reimport(UObject* Obj) override;
	virtual bool IsAutomatedImport() const override { return true; }
};
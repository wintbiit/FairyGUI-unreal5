// Fill out your copyright notice in the Description page of Project Settings.


#include "UIPackageFactory.h"

#include "EditorFramework/AssetImportData.h"
#include "UI/UIPackage.h"
#include "Utils/ByteBuffer.h"

UUIPackageFactory::UUIPackageFactory()
{
	SupportedClass = UUIPackage::StaticClass();
	bEditorImport = true;
	bCreateNew = false;
	bText = false;
	Formats.Add(TEXT("fui;FairyGUI package files"));
}

UObject* UUIPackageFactory::FactoryCreateBinary(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn)
{
	UUIPackage* Package = NewObject<UUIPackage>(InParent, InName, Flags);
	FByteBuffer Buf(Buffer, 0, BufferEnd - Buffer, false);
	Package->Load(&Buf);
	if (!Package->AssetImportData)
	{
		Package->AssetImportData = NewObject<UAssetImportData>(Package, UAssetImportData::StaticClass());
	}
	Package->AssetImportData->Update(CurrentFilename);

	return Package;
}

bool UUIPackageFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	if (const UUIPackage* Package = Cast<UUIPackage>(Obj))
	{
		if (Package->AssetImportData)
		{
			Package->AssetImportData->ExtractFilenames(OutFilenames);
			return true;
		}
	}

	return false;
}

void UUIPackageFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	if (const UUIPackage* Package = Cast<UUIPackage>(Obj))
	{
		if (ensure(NewReimportPaths.Num() == 1))
		{
			Package->AssetImportData->UpdateFilenameOnly(NewReimportPaths[0]);
		}
	}
}

EReimportResult::Type UUIPackageFactory::Reimport(UObject* Obj)
{
	UUIPackage* UIAsset = Cast<UUIPackage>(Obj);
	if (!UIAsset)
	{
		return EReimportResult::Failed;
	}

	const FString Filename = UIAsset->AssetImportData->GetFirstFilename();

	if (!Filename.Len() || IFileManager::Get().FileSize(*Filename) == INDEX_NONE)
	{
		return EReimportResult::Failed;
	}

	if (StaticImportObject(
		UIAsset->GetClass(),
		UIAsset->GetOuter(),
		*UIAsset->GetName(),
		RF_Public | RF_Standalone,
		*Filename,
		nullptr,
		this))
	{
		if (UIAsset->GetOuter())
		{
			UIAsset->GetOuter()->MarkPackageDirty();
		}
		else
		{
			UIAsset->MarkPackageDirty();
		}

		return EReimportResult::Succeeded;
	}
	return EReimportResult::Failed;
}

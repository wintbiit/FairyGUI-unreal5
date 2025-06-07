#include "UIPackageAsset.h"
#include "EditorFramework/AssetImportData.h"

FPrimaryAssetType UUIPackageAsset::AssetType(TEXT("UIPackage"));

#if WITH_EDITORONLY_DATA
void UUIPackageAsset::GetAssetRegistryTags(FAssetRegistryTagsContext Context) const
{
    if (AssetImportData)
    {
        Context.AddTag(FAssetRegistryTag(SourceFileTagName(),
            AssetImportData->GetSourceData().ToJson(),
            FAssetRegistryTag::TT_Hidden));
#if WITH_EDITOR
        AssetImportData->AppendAssetRegistryTags(Context);
#endif
    }

    Super::GetAssetRegistryTags(Context);
}
#endif
#include "UIPackageAsset.h"
#include "EditorFramework/AssetImportData.h"

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
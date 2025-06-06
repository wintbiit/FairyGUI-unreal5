#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UIPackage.generated.h"

class FPackageItem;
class UGObject;
class FByteBuffer;
class UUIPackageAsset;

USTRUCT(BlueprintType)
struct FUIPackageDependency
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
    FPrimaryAssetId AssetId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
    FString Name;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UUIPackage final : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (WorldContext = "WorldContextObject"))
    static const FString& GetBranch();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (WorldContext = "WorldContextObject"))
    static void SetBranch(const FString& InBranch);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (DisplayName = "Get UI Global Variable", WorldContext = "WorldContextObject"))
    static FString GetVar(const FString& VarKey);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (DisplayName = "Set UI Global Variable", WorldContext = "WorldContextObject"))
    static void SetVar(const FString& VarKey, const FString& VarValue);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (WorldContext = "WorldContextObject"))
    static UUIPackage* AddPackageByPath(const FString& InAssetPath);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (WorldContext = "WorldContextObject"))
    static UUIPackage* AddPackage(class UUIPackageAsset* InAsset);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (WorldContext = "WorldContextObject"))
    static void RemovePackage(const FString& IDOrName);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (WorldContext = "WorldContextObject"))
    static void RemoveAllPackages();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    static UUIPackage* GetPackageByID(const FString& PackageID);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    static UUIPackage* GetPackageByName(const FString& PackageName);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (DisplayName = "Create UI", DeterminesOutputType = "ClassType", WorldContext = "WorldContextObject"))
    static UGObject* CreateObject(const FString& PackageName, const FString& ResourceName, UObject* WorldContextObject, TSubclassOf<UGObject> ClassType = nullptr);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (DisplayName = "Create UI From URL", DeterminesOutputType = "ClassType", WorldContext = "WorldContextObject"))
    static UGObject* CreateObjectFromURL(const FString& URL, UObject* WorldContextObject, TSubclassOf<UGObject> ClassType = nullptr);
    
    UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (WorldContext = "WorldContextObject"))
    static void RegisterFont(const FString& FontFace, UFont* Font, UObject* WorldContextObject);

    static FString GetItemURL(const FString& PackageName, const FString& ResourceName);
    static TSharedPtr<FPackageItem> GetItemByURL(const FString& URL);
    static FString NormalizeURL(const FString& URL);

    static int32 Constructing;

public:
    UUIPackage();
    virtual  ~UUIPackage() override;

    static FPrimaryAssetType UIPackageType;
    
    virtual FPrimaryAssetId GetPrimaryAssetId() const override
    {
        return FPrimaryAssetId(UIPackageType, *ID);
    }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    const FString& GetID() const { return ID; }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    const FString& GetName() const { return Name; }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    TArray<FString> GetBranches() const { return Branches; }
    
    TSharedPtr<FPackageItem> GetItem(const FString& ResourceID) const;
    TSharedPtr<FPackageItem> GetItemByName(const FString& ResourceName);
    void* GetItemAsset(const TSharedPtr<FPackageItem>& Item);

    UGObject* CreateObject(const FString& ResourceName, UObject* WorldContextObject);
    UGObject* CreateObject(const TSharedPtr<FPackageItem>& Item, UObject* WorldContextObject);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FairyGUI")
    FString ID;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FairyGUI")
    FString Name;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FairyGUI")
    FString CustomID;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FairyGUI")
    TArray<FString> Branches;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FairyGUI")
    int32 BranchIndex;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FairyGUI")
    TMap<FString, FPrimaryAssetId> Dependencies;

#if WITH_EDITORONLY_DATA
    UPROPERTY(VisibleAnywhere, Instanced, Category=ImportSettings)
    TObjectPtr<UAssetImportData> AssetImportData;

    virtual void GetAssetRegistryTags(FAssetRegistryTagsContext Context) const override;
#endif
    
private:
    void Load(FByteBuffer* Buffer);
    void LoadAtlas(const TSharedPtr<FPackageItem>& Item);
    void LoadImage(const TSharedPtr<FPackageItem>& Item);
    void LoadMovieClip(const TSharedPtr<FPackageItem>& Item);
    void LoadFont(const TSharedPtr<FPackageItem>& Item);
    void LoadSound(const TSharedPtr<FPackageItem>& Item);
    
    TArray<TSharedPtr<FPackageItem>> Items;
    TMap<FString, TSharedPtr<FPackageItem>> ItemsByID;
    TMap<FString, TSharedPtr<FPackageItem>> ItemsByName;
    TMap<FString, struct FAtlasSprite*> Sprites;

    friend class FPackageItem;
    friend class UFairyApplication;
    friend class UUIPackageFactory;
};

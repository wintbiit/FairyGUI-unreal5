#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/GameViewportClient.h"
#include "Engine.h"
#include "Framework/Application/IInputProcessor.h"
#include "Event/EventContext.h"
#include "Tween/TweenManager.h"
#include "UI/UIConfig.h"
#include "FairyApplication.generated.h"

class UUIPackage;
class UNTexture;
class UGObject;
class UGRoot;
class UDragDropManager;

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyApplication : public UWorldSubsystem
{
	GENERATED_BODY()

private:
	struct FTouchInfo
	{
		int32 UserIndex;
		int32 PointerIndex;
		bool bDown;
		bool bToClearCaptors;
		FVector2D DownPosition;
		bool bClickCancelled;
		int32 ClickCount;
		TArray<TWeakPtr<SWidget>> DownPath;
		TArray<TWeakObjectPtr<UGObject>> MouseCaptors;
		FPointerEvent Event;

		FTouchInfo();
	};

	class FInputProcessor : public IInputProcessor
	{
	public:
		FInputProcessor(UFairyApplication* InApplication);
		virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;
		virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
		virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
		virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;

		UFairyApplication* Application;
	};

public:
	UFUNCTION(BlueprintCallable, Category="FariGUI")
	static UFairyApplication* Get(const UObject* WorldContext)
	{
		return WorldContext ? WorldContext->GetWorld()->GetSubsystem<UFairyApplication>() : nullptr;
	}

	UFairyApplication();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UGRoot* GetUIRoot() const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UDragDropManager* GetDragDropManager() const { return DragDropManager; }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FVector2D GetTouchPosition(int32 InUserIndex = -1, int32 InPointerIndex = -1);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	int32 GetTouchCount() const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UGObject* GetObjectUnderPoint(const FVector2D& ScreenspacePosition);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void CancelClick(int32 InUserIndex = -1, int32 InPointerIndex = -1);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void PlaySound(const FString& URL, float VolumeScale = 1);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	bool IsSoundEnabled() const { return bSoundEnabled; }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetSoundEnabled(bool InEnabled);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	float GetSoundVolumeScale() const { return SoundVolumeScale; }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetSoundVolumeScale(float InVolumeScale);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	static FUIConfig& GetUIConfig()
	{
		return UIConfig;
	}

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	static void SetUIConfig(const FUIConfig& InConfig)
	{
		UIConfig = InConfig;
	}
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override
	{
		return GetWorld() && GetWorld()->IsGameWorld() && FSlateApplication::IsInitialized();
	}

	bool DispatchEvent(const FName& EventType, const TSharedRef<SWidget>& Initiator,
	                   const FNVariant& Data = FNVariant::Null);
	void BubbleEvent(const FName& EventType, const TSharedRef<SWidget>& Initiator,
	                 const FNVariant& Data = FNVariant::Null);
	void BroadcastEvent(const FName& EventType, const TSharedRef<SWidget>& Initiator,
	                    const FNVariant& Data = FNVariant::Null);

	void AddMouseCaptor(int32 InUserIndex, int32 InPointerIndex, UGObject* InTarget);
	void RemoveMouseCaptor(int32 InUserIndex, int32 InPointerIndex, UGObject* InTarget);
	bool HasMouseCaptor(int32 InUserIndex, int32 InPointerIndex);

	FReply OnWidgetMouseButtonDown(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry,
	                               const FPointerEvent& MouseEvent);
	FReply OnWidgetMouseButtonUp(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry,
	                             const FPointerEvent& MouseEvent);
	FReply OnWidgetMouseMove(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry,
	                         const FPointerEvent& MouseEvent);
	FReply OnWidgetMouseButtonDoubleClick(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry,
	                                      const FPointerEvent& MouseEvent);
	void OnWidgetMouseEnter(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry,
	                        const FPointerEvent& MouseEvent);
	void OnWidgetMouseLeave(const TSharedRef<SWidget>& Widget, const FPointerEvent& MouseEvent);
	FReply OnWidgetMouseWheel(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry,
	                          const FPointerEvent& MouseEvent);

	UGameViewportClient* GetViewportClient()
	{
		if (!CachedViewportClient)
		{
			CachedViewportClient = GetWorld()->GetGameViewport();
		}

		return CachedViewportClient;
	}
	TSharedPtr<SWidget> GetViewportWidget() { return GetViewportClient()->GetGameViewportWidget(); }

	void CallAfterSlateTick(FSimpleDelegate Callback);

	template <class UserClass, typename... VarTypes>
	void DelayCall(FTimerHandle& InOutHandle, UserClass* InUserObject,
	               typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type inTimerMethod, VarTypes...);
	void CancelDelayCall(FTimerHandle& InHandle) const;

private:
	void PreviewDownEvent(const FPointerEvent& MouseEvent);
	void PreviewUpEvent(const FPointerEvent& MouseEvent);
	void PreviewMoveEvent(const FPointerEvent& MouseEvent);

	UEventContext* BorrowEventContext();
	void ReturnEventContext(UEventContext* Context);

	void InternalBubbleEvent(const FName& EventType, const TArray<UGObject*>& CallChain, const FNVariant& Data);

	FTouchInfo* GetTouchInfo(const FPointerEvent& MouseEvent);
	FTouchInfo* GetTouchInfo(int32 InUserIndex, int32 InPointerIndex);

	void OnSlatePostTick(float DeltaTime);

private:
	UPROPERTY(Transient)
	TObjectPtr<UGRoot> UIRoot;
	UPROPERTY(Transient)
	TObjectPtr<UDragDropManager> DragDropManager;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UEventContext>> EventContextPool;
	UPROPERTY(Transient)
	TObjectPtr<UGameViewportClient> CachedViewportClient;

	TSharedPtr<IInputProcessor> InputProcessor;
	TIndirectArray<FTouchInfo> Touches;
	FTouchInfo* LastTouch;
	bool bNeedCheckPopups;
	FDelegateHandle PostTickDelegateHandle;
	FSimpleMulticastDelegate PostTickMulticastDelegate;
	bool bSoundEnabled;
	float SoundVolumeScale;

public:
	static FUIConfig UIConfig;
	static FString Branch;
	static TArray<TObjectPtr<UUIPackage>> PackageList;
	static TMap<FString, TObjectPtr<UUIPackage>> PackageInstByID;
	static TMap<FString, TObjectPtr<UUIPackage>> PackageInstByName;
	static TMap<FString, TObjectPtr<UFont>> Fonts;
	static TMap<FString, FString> Vars;
};

template <class UserClass, typename... VarTypes>
void UFairyApplication::DelayCall(FTimerHandle& InOutHandle, UserClass* InUserObject,
                                  typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type inTimerMethod,
                                  VarTypes... Vars)
{
	if (!GetWorld()->GetTimerManager().TimerExists(InOutHandle))
		InOutHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateUObject(InUserObject, inTimerMethod, Vars...));
}

inline void UFairyApplication::CancelDelayCall(FTimerHandle& InHandle) const
{
	GetWorld()->GetTimerManager().ClearTimer(InHandle);
}

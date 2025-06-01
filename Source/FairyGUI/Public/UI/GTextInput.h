#pragma once

#include "GObject.h"
#include "GTextField.h"
#include "Widgets/NTextFormat.h"
#include "GTextInput.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTextChangedEvent, const FText&, Text);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTextCommittedEvent, const FText&, Text, ETextCommit::Type, CommitMethod);

UCLASS(BlueprintType)
class FAIRYGUI_API UGTextInput : public UGTextField
{
	GENERATED_BODY()

public:
	UGTextInput();
	virtual ~UGTextInput() override;

	virtual const FString& GetText() const override { return Text; }
	virtual void SetText(const FString& InText) override;

	TSharedRef<SMultiLineEditableText> GetInputWidget() const;

	virtual bool IsSingleLine() const override;

	virtual void SetSingleLine(bool InSingleLine) override;

	virtual FNTextFormat& GetTextFormat() override { return TextFormat; }

	virtual void SetTextFormat(const FNTextFormat& InTextFormat) override;

	virtual void ApplyFormat() override;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetPrompt(const FString& InPrompt);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetPassword(bool bInPassword);

	//仅能输入数字（使用正则表达式时不生效）
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetOnlyNumeric(bool bInOnlyNumeric);

	//使用正则表达式（传入空值即为不再使用正则）
	UFUNCTION(BlueprintCallable, Category= "FairyGUI")
	void SetRegexPattern(FString InRegexPattern);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetKeyboardType(int32 InKeyboardType);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetMaxLength(int32 InMaxLength);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetRestrict(const FString& InRestrict);

	UFUNCTION(BlueprintPure, Category = "FairyGUI")
	int32 GetMaxLength() const;
	
	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FGUIEventDynMDelegate OnSubmit;

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FOnTextChangedEvent OnTextChanged;

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FOnTextCommittedEvent OnTextCommitted;

	virtual FNVariant GetProp(EObjectPropID PropID) const override;
	virtual void SetProp(EObjectPropID PropID, const FNVariant& InValue) override;

	void NotifyTextChanged(const FText& InText);

protected:
	virtual bool IsValidText(const FText& InText);
	virtual void SetupBeforeAdd(FByteBuffer* Buffer, int32 BeginPos) override;
	virtual void SetupAfterAdd(FByteBuffer* Buffer, int32 BeginPos) override;

	FString Text;
	bool bFormatApplied;
	FNTextFormat TextFormat;

	TSharedPtr<class STextInput> Content;

	int32 MaxLength = -1;

	bool bOnlyNumeric = false;

	bool bHasRegexPattern = false;
	FRegexPattern RegexPattern = FRegexPattern("");
};

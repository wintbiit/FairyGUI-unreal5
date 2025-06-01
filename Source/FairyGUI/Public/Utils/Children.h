// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SNullWidget.h"
#include "SlotBase.h"
#include "Layout/ChildrenBase.h"
#include "Layout/BasicLayoutWidgetSlot.h"
#include "Widgets/SWidget.h"

/**
 * A generic FChildren that stores children along with layout-related information.
 * The type containing Widget* and layout info is specified by ChildType.
 * ChildType must have a public member SWidget* Widget;
 */
template<typename SlotType>
class TPanelChildrenLegacy final : public FChildren
{
private:
	TArray<TUniquePtr<SlotType>> Children;
	static constexpr bool bSupportSlotWithSlateAttribute = std::is_base_of<TWidgetSlotWithAttributeSupport<SlotType>, SlotType>::value;

protected:
	virtual const FSlotBase& GetSlotAt(int32 ChildIndex) const override
	{
		return *Children[ChildIndex];
	}

	virtual FWidgetRef GetChildRefAt(int32 ChildIndex) override
	{
		return FWidgetRef(ReferenceConstruct, Children[ChildIndex]->GetWidget().Get());
	}

	virtual FConstWidgetRef GetChildRefAt(int32 ChildIndex) const override
	{
		return FConstWidgetRef(ReferenceConstruct, Children[ChildIndex]->GetWidget().Get());
	}

public:
	using FChildren::FChildren;
	
	virtual int32 Num() const override
	{
		return Children.Num();
	}

	virtual TSharedRef<SWidget> GetChildAt( int32 Index ) override
	{
		return Children[Index]->GetWidget();
	}

	virtual TSharedRef<const SWidget> GetChildAt( int32 Index ) const override
	{
		return Children[Index]->GetWidget();
	}

	virtual bool SupportSlotWithSlateAttribute() const override
	{
		return bSupportSlotWithSlateAttribute;
	}

public:
	UE_DEPRECATED(5.0, "Add a slot directly has been deprecated. use the FSlotArgument to create a new slot")
	int32 Add( SlotType* Slot )
	{
		int32 Index = Children.Add(TUniquePtr<SlotType>(Slot));
		check(Slot);
		Slot->SetOwner(*this);

		return Index;
	}

	int32 AddSlot(typename SlotType::FSlotArguments&& SlotArgument)
	{
		TUniquePtr<SlotType> NewSlot = SlotArgument.StealSlot();
		check(NewSlot.Get());
		int32 Result = Children.Add(MoveTemp(NewSlot));
		Children[Result]->Construct(*this, MoveTemp(SlotArgument));
		return Result;
	}

	void AddSlots(TArray<typename SlotType::FSlotArguments> SlotArguments)
	{
		Children.Reserve(Children.Num() + SlotArguments.Num());
		for (typename SlotType::FSlotArguments& Arg : SlotArguments)
		{
			AddSlot(MoveTemp(Arg));
		}
	}

	void RemoveAt( int32 Index )
	{
		// NOTE:
		// We don't do any invalidating here, that's handled by the FSlotBase, which eventually calls ConditionallyDetatchParentWidget

		// Steal the instance from the array, then free the element.
		// This alleviates issues where (misbehaving) destructors on the children may call back into this class and query children while they are being destroyed.
		TUniquePtr<SlotType> SlotToRemove = MoveTemp(Children[Index]);
		Children.RemoveAt(Index);
		SlotToRemove.Reset();
	}

	/** Removes the corresponding widget from the set of children if it exists.  Returns the index it found the child at, INDEX_NONE otherwise. */
	int32 Remove(const TSharedRef<SWidget>& SlotWidget)
	{
		for (int32 SlotIdx = 0; SlotIdx < Num(); ++SlotIdx)
		{
			if (SlotWidget == Children[SlotIdx]->GetWidget())
			{
				Children.RemoveAt(SlotIdx);
				return SlotIdx;
			}
		}

		return INDEX_NONE;
	}

	void Empty(int32 Slack = 0)
	{
		// NOTE:
		// We don't do any invalidating here, that's handled by the FSlotBase, which eventually calls ConditionallyDetatchParentWidget

		// We empty children by first transferring them onto a stack-owned array, then freeing the elements.
		// This alleviates issues where (misbehaving) destructors on the children may call back into this class and query children while they are being destroyed.
		// By storing the children on the stack first, we defer the destruction of children until after we have emptied our owned container.
		TArray<TUniquePtr<SlotType>> ChildrenCopy = MoveTemp(Children);

		// Explicitly calling Empty is not really necessary (it is already empty/moved-from now), but we call it for safety
		Children.Empty();

		// ChildrenCopy will now be emptied and moved back (to preserve any allocated memory)
		ChildrenCopy.Empty(Slack);
		Children = MoveTemp(ChildrenCopy);
	}

	UE_DEPRECATED(5.0, "Insert a slot directly has been deprecated. use the FSlotArgument to create a new slot")
	void Insert(SlotType* Slot, int32 Index)
	{
		check(Slot);
		Children.Insert(TUniquePtr<SlotType>(Slot), Index);
		Slot->SetOwner(*this);
	}

	void InsertSlot(typename SlotType::FSlotArguments&& SlotArgument, int32 Index)
	{
		TUniquePtr<SlotType> NewSlot = SlotArgument.StealSlot();
		check(NewSlot.Get());
		Children.Insert(MoveTemp(NewSlot), Index);
		Children[Index]->Construct(*this, MoveTemp(SlotArgument));
	}

	void Move(int32 IndexToMove, int32 IndexToDestination)
	{
		{
			TUniquePtr<SlotType> SlotToMove = MoveTemp(Children[IndexToMove]);
			Children.RemoveAt(IndexToMove);
			Children.Insert(MoveTemp(SlotToMove), IndexToDestination);
			if constexpr (bSupportSlotWithSlateAttribute)
			{
				check(Children.Num() > 0);
				Children[0]->RequestSortAttribute();
			}
		}

		GetOwner().Invalidate(EInvalidateWidgetReason::ChildOrder);
	}

	void Reserve( int32 NumToReserve )
	{
		Children.Reserve(NumToReserve);
	}

	bool IsValidIndex( int32 Index ) const
	{
		return Children.IsValidIndex( Index );
	}

	const SlotType& operator[](int32 Index) const
	{
		return *Children[Index];
	}
	SlotType& operator[](int32 Index)
	{
		return *Children[Index];
	}

	template <class PREDICATE_CLASS>
	void Sort( const PREDICATE_CLASS& Predicate )
	{
		if (Children.Num() > 0)
		{
			Children.Sort([&Predicate](const TUniquePtr<SlotType>& One, const TUniquePtr<SlotType>& Two)
			{
				return Predicate(*One, *Two);
			});

			if constexpr (bSupportSlotWithSlateAttribute)
			{
				Children[0]->RequestSortAttribute();
			}

			GetOwner().Invalidate(EInvalidateWidgetReason::ChildOrder);
		}
	}

	template <class PREDICATE_CLASS>
	void StableSort(const PREDICATE_CLASS& Predicate)
	{
		if (Children.Num() > 0)
		{
			Children.StableSort([&Predicate](const TUniquePtr<SlotType>& One, const TUniquePtr<SlotType>& Two)
			{
				return Predicate(*One, *Two);
			});

			if constexpr (bSupportSlotWithSlateAttribute)
			{
				Children[0]->RequestSortAttribute();
			}

			GetOwner().Invalidate(EInvalidateWidgetReason::ChildOrder);
		}
	}

	void Swap( int32 IndexA, int32 IndexB )
	{
		Children.Swap(IndexA, IndexB);

		if constexpr (bSupportSlotWithSlateAttribute)
		{
			check(Children.Num() > 0);
			Children[0]->RequestSortAttribute();
		}

		GetOwner().Invalidate(EInvalidateWidgetReason::ChildOrder);
	}

public:
	/** At the end of the scope a slot will be constructed and added to the FChildren. */
	struct FScopedWidgetSlotArguments : public SlotType::FSlotArguments
	{
	public:
		FScopedWidgetSlotArguments(TUniquePtr<SlotType> InSlot, TPanelChildren<SlotType>& InChildren, int32 InIndex)
			: SlotType::FSlotArguments(MoveTemp(InSlot))
			, Children(InChildren)
			, Index(InIndex)
		{
		}
		FScopedWidgetSlotArguments(TUniquePtr<SlotType> InSlot, TPanelChildren<SlotType>& InChildren, int32 InIndex, TFunction<void(const SlotType*, int32)> OnAdded)
			: SlotType::FSlotArguments(MoveTemp(InSlot))
			, Children(InChildren)
			, Index(InIndex)
			, Added(OnAdded)
		{
		}
	
		FScopedWidgetSlotArguments(const FScopedWidgetSlotArguments&) = delete;
		FScopedWidgetSlotArguments& operator=(const FScopedWidgetSlotArguments&) = delete;
		FScopedWidgetSlotArguments(FScopedWidgetSlotArguments&&) = default;
		FScopedWidgetSlotArguments& operator=(FScopedWidgetSlotArguments&&) = default;
	
		virtual ~FScopedWidgetSlotArguments()
		{
			if (const SlotType* SlotPtr = this->GetSlot())	// Is nullptr when the FScopedWidgetSlotArguments was moved-constructed.
			{
				if (Index == INDEX_NONE)
				{
					Index = Children.AddSlot(MoveTemp(*this));
				}
				else
				{
					Children.InsertSlot(MoveTemp(*this), Index);
				}
				if (Added)
				{
					Added(SlotPtr, Index);
				}
			}
		}

	private:
		TPanelChildren<SlotType>& Children;
		int32 Index;
		TFunction<void(const SlotType*, int32)> Added;
	};
};


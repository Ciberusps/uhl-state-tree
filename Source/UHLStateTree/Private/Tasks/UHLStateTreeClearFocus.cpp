// Pavel Penkov 2025 All Rights Reserved.

#include "Tasks/UHLStateTreeClearFocus.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "Components/UHLStateTreeAIComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UHLStateTreeClearFocus)

#define LOCTEXT_NAMESPACE "UHLStateTree"

EStateTreeRunStatus FUHLStateTreeClearFocus::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	const UWorld* World = Context.GetWorld();

	// Reference actor is not required (offset will be used as a global world location)
	// but a valid world is required.
	if (World == nullptr)
	{
		return EStateTreeRunStatus::Failed;
	}

	AAIController* AIController = InstanceData.AIController;
	if (!AIController) return EStateTreeRunStatus::Failed;

	AIController->ClearFocus(static_cast<uint8>(InstanceData.FocusPriority));

	return EStateTreeRunStatus::Succeeded;
}

#if WITH_EDITOR
FText FUHLStateTreeClearFocus::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// Get priority as string or binding
	FString PriorityStr;
	FStateTreePropertyPath PriorityPath(ID, GET_MEMBER_NAME_CHECKED(FUHLStateTreeClearFocusInstanceData, FocusPriority));
	FText PriorityBindingSource = BindingLookup.GetBindingSourceDisplayName(PriorityPath);
	if (!PriorityBindingSource.IsEmpty())
	{
		PriorityStr = FString::Printf(TEXT("Bound: %s"), *PriorityBindingSource.ToString());
	}
	else
	{
		switch (InstanceData->FocusPriority)
		{
		case EUHLAIFocusPriority::Default:
			PriorityStr = TEXT("Default");
			break;
		case EUHLAIFocusPriority::Move:
			PriorityStr = TEXT("Move");
			break;
		case EUHLAIFocusPriority::Gameplay:
			PriorityStr = TEXT("Gameplay");
			break;
		default:
			PriorityStr = TEXT("Unknown");
			break;
		}
	}

	// bFinishTask binding
	FString FinishStr;
	FStateTreePropertyPath FinishPath(ID, GET_MEMBER_NAME_CHECKED(FUHLStateTreeClearFocusInstanceData, bFinishTask));
	FText FinishBindingSource = BindingLookup.GetBindingSourceDisplayName(FinishPath);
	bool bFinishTask = InstanceData->bFinishTask;
	if (!FinishBindingSource.IsEmpty())
	{
		FinishStr = FString::Printf(TEXT("Bound: %s"), *FinishBindingSource.ToString());
	}
	else
	{
		FinishStr = bFinishTask ? TEXT("Finish Task") : TEXT("Continue");
	}

	// Build the description
	FString Description = FString::Printf(TEXT("Clear Focus (Priority: %s) [%s]"), *PriorityStr, *FinishStr);

	if (Formatting == EStateTreeNodeFormatting::RichText)
	{
		return FText::FromString(FString::Printf(TEXT("<b>%s</>"), *Description));
	}

	return FText::FromString(FString::Printf(TEXT(">%s"), *Description));
}

FName FUHLStateTreeClearFocus::GetIconName() const
{
	return FName("Icons.Cross");
}

FColor FUHLStateTreeClearFocus::GetIconColor() const
{
	return UE::StateTree::Colors::Grey;
}
#endif

#undef LOCTEXT_NAMESPACE
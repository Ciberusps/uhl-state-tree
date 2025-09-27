// Pavel Penkov 2025 All Rights Reserved.

#include "Tasks/UHLSTTask_ClearFocus.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "Components/UHLStateTreeAIComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UHLSTTask_ClearFocus)

#define LOCTEXT_NAMESPACE "UHLSTTask_ClearFocus"

EStateTreeRunStatus FUHLSTTask_ClearFocus::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
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

	return InstanceData.bFinishTask ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FUHLSTTask_ClearFocus::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// Get priority as string or binding
	FString PriorityStr;
	const FPropertyBindingPath PriorityPath(ID, GET_MEMBER_NAME_CHECKED(FUHLSTTask_ClearFocusInstanceData, FocusPriority));
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
	const FPropertyBindingPath FinishPath(ID, GET_MEMBER_NAME_CHECKED(FUHLSTTask_ClearFocusInstanceData, bFinishTask));
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

FName FUHLSTTask_ClearFocus::GetIconName() const
{
	return FName("Icons.Cross");
}

FColor FUHLSTTask_ClearFocus::GetIconColor() const
{
	return UE::StateTree::Colors::Grey;
}
#endif

#undef LOCTEXT_NAMESPACE
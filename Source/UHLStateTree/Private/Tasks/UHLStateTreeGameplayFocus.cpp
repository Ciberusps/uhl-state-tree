// Pavel Penkov 2025 All Rights Reserved.

#include "Tasks/UHLStateTreeGameplayFocus.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "Components/UHLStateTreeAIComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UHLStateTreeGameplayFocus)

#define LOCTEXT_NAMESPACE "UHLStateTree"

EStateTreeRunStatus FUHLStateTreeGameplayFocus::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
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

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FUHLStateTreeGameplayFocus::Tick(
	FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.AIController || !InstanceData.bEnable)
	{
		return FStateTreeTaskCommonBase::Tick(Context, DeltaTime);
	}

	if (InstanceData.ActorToFocus)
	{
		InstanceData.AIController->SetFocus(InstanceData.ActorToFocus, static_cast<uint8>(InstanceData.FocusPriority));	
	}
	else
	{
		InstanceData.AIController->SetFocalPoint(InstanceData.LocationToFocus, static_cast<uint8>(InstanceData.FocusPriority));	
	}
	
	return FStateTreeTaskCommonBase::Tick(Context, DeltaTime);
}

#if WITH_EDITOR
FText FUHLStateTreeGameplayFocus::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// Determine focus target description
	FString FocusTarget;
	FStateTreePropertyPath ActorPath(ID, GET_MEMBER_NAME_CHECKED(FUHLStateTreeGameplayFocusInstanceData, ActorToFocus));
	FText ActorBindingSource = BindingLookup.GetBindingSourceDisplayName(ActorPath);
	if (!ActorBindingSource.IsEmpty())
	{
		FocusTarget = FString::Printf(TEXT("Actor (Bound: %s)"), *ActorBindingSource.ToString());
	}
	else if (!InstanceData->ActorToFocus.Get())
	{
		FString ActorName = TEXT("Selected Actor");
		if (InstanceData->ActorToFocus.Get())
		{
			ActorName = InstanceData->ActorToFocus->GetName();
		}
		FocusTarget = FString::Printf(TEXT("Actor (%s)"), *ActorName);
	}
	else
	{
		FStateTreePropertyPath LocationPath(ID, GET_MEMBER_NAME_CHECKED(FUHLStateTreeGameplayFocusInstanceData, LocationToFocus));
		FText LocationBindingSource = BindingLookup.GetBindingSourceDisplayName(LocationPath);
		if (!LocationBindingSource.IsEmpty())
		{
			FocusTarget = FString::Printf(TEXT("Location (Bound: %s)"), *LocationBindingSource.ToString());
		}
		else
		{
			FocusTarget = FString::Printf(TEXT("Location (%s)"), *InstanceData->LocationToFocus.ToString());
		}
	}

	// Get priority as string or binding
	FString PriorityStr;
	FStateTreePropertyPath PriorityPath(ID, GET_MEMBER_NAME_CHECKED(FUHLStateTreeGameplayFocusInstanceData, FocusPriority));
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

	// bEnable binding
	FString EnableStr;
	FStateTreePropertyPath EnablePath(ID, GET_MEMBER_NAME_CHECKED(FUHLStateTreeGameplayFocusInstanceData, bEnable));
	FText EnableBindingSource = BindingLookup.GetBindingSourceDisplayName(EnablePath);
	bool bEnable = InstanceData->bEnable;
	if (!EnableBindingSource.IsEmpty())
	{
		EnableStr = FString::Printf(TEXT("Bound: %s"), *EnableBindingSource.ToString());
	}
	else
	{
		EnableStr = bEnable ? TEXT("Enabled") : TEXT("Disabled");
	}

	// Build the description
	FString Description;
	if (!bEnable && EnableBindingSource.IsEmpty())
	{
		Description = TEXT("Disabled (No Focus Action)");
	}
	else
	{
		Description = FString::Printf(TEXT("Focus on %s (Priority: %s) [%s]"), *FocusTarget, *PriorityStr, *EnableStr);
	}

	if (Formatting == EStateTreeNodeFormatting::RichText)
	{
		return FText::FromString(FString::Printf(TEXT("<b>%s</>"), *Description));
	}

	return FText::FromString(FString::Printf(TEXT(">%s"), *Description));
}

FName FUHLStateTreeGameplayFocus::GetIconName() const
{
	return FName("BTEditor.Graph.BTNode.Service.DefaultFocus.Icon");
}

FColor FUHLStateTreeGameplayFocus::GetIconColor() const
{
	return UE::StateTree::Colors::Grey;
}
#endif

#undef LOCTEXT_NAMESPACE
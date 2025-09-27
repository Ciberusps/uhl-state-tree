// Pavel Penkov 2025 All Rights Reserved.

#pragma once

#include "AIController.h"
#include "StateTreeTaskBase.h"
#include "UHLSTTask_GameplayFocus.h"
#include "UHLSTTask_ClearFocus.generated.h"

enum class EStateTreeRunStatus : uint8;
struct FStateTreeTransitionResult;

USTRUCT()
struct UHLSTATETREE_API FUHLSTTask_ClearFocusInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> AIController = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	EUHLAIFocusPriority FocusPriority = EUHLAIFocusPriority::Gameplay;
	
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bFinishTask = true;
};

USTRUCT(meta = (DisplayName = "ClearFocus", Category="UHLStateTree"))
struct UHLSTATETREE_API FUHLSTTask_ClearFocus : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FUHLSTTask_ClearFocusInstanceData;
	
	FUHLSTTask_ClearFocus() = default;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
	virtual FName GetIconName() const override;
	virtual FColor GetIconColor() const override;
#endif
};

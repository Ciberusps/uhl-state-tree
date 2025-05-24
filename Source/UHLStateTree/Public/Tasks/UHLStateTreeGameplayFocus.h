// Pavel Penkov 2025 All Rights Reserved.

#pragma once

#include "AIController.h"
#include "StateTreeTaskBase.h"
#include "AITypes.h"
#include "UHLStateTreeGameplayFocus.generated.h"

enum class EStateTreeRunStatus : uint8;
struct FStateTreeTransitionResult;

UENUM(BlueprintType)
enum class EUHLAIFocusPriority : uint8
{
	Default = 0 UMETA(DisplayName = "Default"),
	Gameplay = 1 UMETA(DisplayName = "Gameplay"),
	Logic = 2 UMETA(DisplayName = "Logic"),
	High = 3 UMETA(DisplayName = "High")
};

USTRUCT()
struct UHLSTATETREE_API FUHLStateTreeGameplayFocusInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> AIController = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<AActor> ActorToFocus = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	FVector LocationToFocus = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	EUHLAIFocusPriority FocusPriority = EUHLAIFocusPriority::Gameplay;
	
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bEnable = true;
};

USTRUCT(meta = (DisplayName = "GameplayFocus"))
struct UHLSTATETREE_API FUHLStateTreeGameplayFocus : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FUHLStateTreeGameplayFocusInstanceData;
	
	FUHLStateTreeGameplayFocus() = default;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
	virtual FName GetIconName() const override;
	virtual FColor GetIconColor() const override;
#endif
};

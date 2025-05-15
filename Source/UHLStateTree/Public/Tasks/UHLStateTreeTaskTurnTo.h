// Pavel Penkov 2025 All Rights Reserved.

#pragma once

#include "AIController.h"
#include "StateTreeTaskBase.h"
#include "UnrealHelperLibraryTypes.h"
#include "Data/TurnSettings.h"
#include "UHLStateTreeTaskTurnTo.generated.h"

enum class EStateTreeRunStatus : uint8;
struct FStateTreeTransitionResult;

USTRUCT()
struct UHLSTATETREE_API FUHLStateTreeTaskTurnToInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> AIController = nullptr;
	
	// The query will be run with this actor has the owner object.
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<ACharacter> Character = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	FVector TargetLocation = FVector::ZeroVector;
	
	UPROPERTY(config, Category="TurnTo", EditAnywhere, meta = (ClampMin = "0.0", Units="Degrees"))
    float Precision = 1.0f;

    UPROPERTY(EditAnywhere, Category="TurnTo")
    bool bUseTurnAnimations = true;
    UPROPERTY(EditAnywhere, Category="TurnTo", meta=(EditCondition="bUseTurnAnimations", EditConditionHides))
    EUHLSettingsSource SettingsSource = EUHLSettingsSource::Actor;

    UPROPERTY(EditAnywhere, Category="TurnTo", meta=(EditCondition="bUseTurnAnimations && SettingsSource==EUHLSettingsSource::Node", EditConditionHides))
    FTurnSettings TurnSettings;
    UPROPERTY(EditAnywhere, Category="TurnTo", meta=(EditCondition="bUseTurnAnimations && SettingsSource==EUHLSettingsSource::DataAsset", EditConditionHides))
    UTurnSettingsDataAsset* RotateToAnimationsDataAsset;

    UPROPERTY(EditAnywhere, Category="TurnTo")
    bool bDebug = false;

	UPROPERTY(EditAnywhere, Category="TurnTo")
	bool bInfinite = false;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bClearFocusOnSucceed = true;
	
	// UPROPERTY(EditAnywhere, Category = "Parameter")
	// bool bFinishTask = true;

    /** cached Precision tangent value */
	UPROPERTY(Transient)
	float PrecisionDot = 0.0f;
	UPROPERTY(Transient)
	FTurnSettings CurrentTurnSettings;
	UPROPERTY(Transient)
	FTurnRange CurrentTurnRange;

	// /** Optional actor where to draw the text at. */
	// UPROPERTY(EditAnywhere, Category = "Input", meta=(Optional))
	// TObjectPtr<AActor> ReferenceActor = nullptr;
};

/**
 * Draws debug text on the HUD associated to the player controller.
 */
USTRUCT(meta = (DisplayName = "Turn To"))
struct UHLSTATETREE_API FUHLStateTreeTaskTurnTo : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FUHLStateTreeTaskTurnToInstanceData;
	
	FUHLStateTreeTaskTurnTo() = default;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
	virtual FName GetIconName() const override
	{
		return FName("StateTreeEditorStyle|Node.Text");
	}
	virtual FColor GetIconColor() const override
	{
		return UE::StateTree::Colors::Grey;
	}
#endif
	
private:
	FTurnSettings GetTurnSettings(FStateTreeExecutionContext& Context, AActor* Actor) const;
};

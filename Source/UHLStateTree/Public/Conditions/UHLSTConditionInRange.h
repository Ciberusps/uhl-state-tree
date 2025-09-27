// Pavel Penkov 2025 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "GameFramework/Character.h"
#include "UHLSTConditionInRange.generated.h"

USTRUCT()
struct UHLSTATETREE_API FUHLSTConditionInRangeInstanceData
{
	GENERATED_BODY()

	// Context character to measure distance from.
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<ACharacter> Character = nullptr;

	// Optional target character. If not valid, Location will be used instead.
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<ACharacter> OtherCharacter = nullptr;

	// Fallback location when OtherCharacter is not provided/invalid.
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FVector Location = FVector::ZeroVector;

	// Distance range to test against. Uses inclusive bounds by default and honors open/exclusive bounds.
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FFloatRange Range = FFloatRange(0.0f, 1000.0f);

	// If true, subtracts self capsule radius from measured distance before range test.
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bIncludeSelfCapsuleRadius = true;

	// If true and OtherCharacter is used, subtracts target capsule radius from measured distance before range test.
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bIncludeTargetCapsuleRadius = true;

	// If true, result is inverted.
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bInverse = false;

	// Enable debug visualization.
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDebug = false;

	// Debug draw duration in seconds. Active only if bDebug is true.
	UPROPERTY(EditAnywhere, Category = "Debug", meta=(ClampMin="0.0", EditCondition="bDebug"))
	float DebugDuration = 2.5f;

	// Optional editor-only description suffix.
	UPROPERTY(EditAnywhere, Category = "Parameter", meta=(MultiLine=true))
	FString Comment;
};

/**
 * InRange condition. Tests if Character is within Range of OtherCharacter or Location.
 */
USTRUCT(meta=(DisplayName="In Range", Category = "UHLStateTree"))
struct UHLSTATETREE_API FUHLSTConditionInRange : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FUHLSTConditionInRangeInstanceData;

	FUHLSTConditionInRange() = default;

	virtual const UStruct* GetInstanceDataType() const override { return FUHLSTConditionInRangeInstanceData::StaticStruct(); }
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
	virtual FName GetIconName() const override
	{
		return FName("StateTreeEditorStyle|Node.Distance");
	}
	virtual FColor GetIconColor() const override
	{
		return UE::StateTree::Colors::Green;
	}
#endif
};



// Pavel Penkov 2025 All Rights Reserved.

#include "Tasks/UHLSTTaskPlayAnimMontage.h"

#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Net/UHLMontageReplicatorObject.h"
#include "StateTreeLinker.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UHLSTTaskPlayAnimMontage)

#define LOCTEXT_NAMESPACE "UHLSTTaskPlayAnimMontage"

USkeletalMeshComponent* FUHLSTTaskPlayAnimMontage::ResolveMesh(const FInstanceDataType& InstanceData) const
{
	if (InstanceData.CustomMesh)
	{
		return InstanceData.CustomMesh;
	}
	return InstanceData.Character ? InstanceData.Character->GetMesh() : nullptr;
}

bool FUHLSTTaskPlayAnimMontage::IsMontagePlaying(USkeletalMeshComponent* Mesh, const UAnimMontage* Montage) const
{
	if (!Mesh) return false;
	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	if (!AnimInstance) return false;
	return AnimInstance->Montage_IsPlaying(Montage);
}

static void UHL_BindMontageDelegates(
    FStateTreeExecutionContext& Context,
    UAnimInstance* AnimInstance,
    UAnimMontage* Montage,
    FUHLSTTaskPlayAnimMontage::FInstanceDataType& InstanceData)
{
	if (!AnimInstance || !Montage) return;
	InstanceData.BoundAnimInstance = AnimInstance;
    FOnMontageEnded Ended;
    Ended.BindLambda([&InstanceData](UAnimMontage* InMontage, bool bInterrupted)
	{
        InstanceData.bInterruptedTriggered |= bInterrupted;
        InstanceData.bCompletedTriggered |= !bInterrupted;
	});
	AnimInstance->Montage_SetEndDelegate(Ended, Montage);

    FOnMontageBlendingOutStarted BlendOut;
    BlendOut.BindLambda([&InstanceData](UAnimMontage* InMontage, bool bInterrupted)
	{
        InstanceData.bBlendOutTriggered = true;
        InstanceData.bInterruptedTriggered |= bInterrupted;
	});
	AnimInstance->Montage_SetBlendingOutDelegate(BlendOut, Montage);
}

EStateTreeRunStatus FUHLSTTaskPlayAnimMontage::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character || !InstanceData.AnimMontage)
	{
		return EStateTreeRunStatus::Failed;
	}

	USkeletalMeshComponent* Mesh = ResolveMesh(InstanceData);
	if (!Mesh)
	{
		return EStateTreeRunStatus::Failed;
	}

	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	if (!AnimInstance)
	{
		return EStateTreeRunStatus::Failed;
	}

    if (InstanceData.bShouldStopAllMontages)
	{
		// Use character API when operating on the main mesh for better replication behavior
		if (Mesh == InstanceData.Character->GetMesh())
		{
			InstanceData.Character->StopAnimMontage();
		}
		else
		{
            if (InstanceData.Character->HasAuthority())
            {
                UUHLMontageReplicatorObject* Replicator = NewObject<UUHLMontageReplicatorObject>(InstanceData.Character);
                Replicator->Initialize(InstanceData.Character);
                InstanceData.Character->AddReplicatedSubObject(Replicator);
                Replicator->Multicast_StopAllMontages(Mesh, 0.25f);
            }
            else
            {
                AnimInstance->StopAllMontages(0.25f);
            }
		}
	}

	// If a starting section is provided, jump to it; otherwise use starting position
	if (Mesh == InstanceData.Character->GetMesh())
	{
		// Playing on Character's main mesh will replicate to simulated proxies when executed on the server
		InstanceData.Character->PlayAnimMontage(InstanceData.AnimMontage, InstanceData.PlayRate);
		if (InstanceData.StartingSection != NAME_None)
		{
			AnimInstance->Montage_JumpToSection(InstanceData.StartingSection, InstanceData.AnimMontage);
		}
		else if (InstanceData.StartingPosition > 0.0f)
		{
			AnimInstance->Montage_SetPosition(InstanceData.AnimMontage, InstanceData.StartingPosition);
		}
	}
    else
	{
        if (InstanceData.Character->HasAuthority())
        {
            UUHLMontageReplicatorObject* Replicator = NewObject<UUHLMontageReplicatorObject>(InstanceData.Character);
            Replicator->Initialize(InstanceData.Character);
            InstanceData.Character->AddReplicatedSubObject(Replicator);
            Replicator->Multicast_PlayMontage(
                Mesh,
                InstanceData.AnimMontage,
                InstanceData.PlayRate,
                InstanceData.StartingPosition,
                InstanceData.StartingSection);
        }
        else
        {
            AnimInstance->Montage_Play(InstanceData.AnimMontage, InstanceData.PlayRate, EMontagePlayReturnType::MontageLength, InstanceData.StartingSection != NAME_None ? 0.0f : InstanceData.StartingPosition, true);
            if (InstanceData.StartingSection != NAME_None)
            {
                AnimInstance->Montage_JumpToSection(InstanceData.StartingSection, InstanceData.AnimMontage);
            }
        }
	}

    // Bind delegates for completion/interrupt/BlendOut and request transition directly
    UHL_BindMontageDelegates(Context, AnimInstance, InstanceData.AnimMontage, InstanceData);

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FUHLSTTaskPlayAnimMontage::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if ((InstanceData.bFinishTaskOnCompleted && InstanceData.bCompletedTriggered)
		|| (InstanceData.bFinishTaskOnInterrupted && InstanceData.bInterruptedTriggered)
		|| (InstanceData.bFinishTaskOnBlendOut && InstanceData.bBlendOutTriggered))
	{
		return InstanceData.bSucceededResult ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Failed;
	}
	return FStateTreeTaskCommonBase::Tick(Context, DeltaTime);
}

void FUHLSTTaskPlayAnimMontage::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (UAnimInstance* AnimInstance = InstanceData.BoundAnimInstance.Get())
	{
		// Clear bound delegates to avoid dangling refs
		FOnMontageEnded EmptyEnded;
		AnimInstance->Montage_SetEndDelegate(EmptyEnded, InstanceData.AnimMontage);
		FOnMontageBlendingOutStarted EmptyBlendOut;
		AnimInstance->Montage_SetBlendingOutDelegate(EmptyBlendOut, InstanceData.AnimMontage);
	}
}

#if WITH_EDITOR
FText FUHLSTTaskPlayAnimMontage::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	FString MontageStr;
	const FPropertyBindingPath MontagePath(ID, GET_MEMBER_NAME_CHECKED(FUHLSTTaskPlayAnimMontageInstanceData, AnimMontage));
	FText MontageBinding = BindingLookup.GetBindingSourceDisplayName(MontagePath);
	if (!MontageBinding.IsEmpty())
	{
		MontageStr = FString::Printf(TEXT("Bound: %s"), *MontageBinding.ToString());
	}
	else
	{
		MontageStr = InstanceData->AnimMontage ? InstanceData->AnimMontage->GetName() : TEXT("None");
	}

    TArray<FString> Parts;
    Parts.Add(FString::Printf(TEXT("Play AnimMontage '%s'"), *MontageStr));

    if (InstanceData->CustomMesh)
    {
        Parts.Add(TEXT("Mesh: CustomMesh"));
    }

    if (!FMath::IsNearlyEqual(InstanceData->PlayRate, 1.0f))
    {
        Parts.Add(FString::Printf(TEXT("Rate %.2f"), InstanceData->PlayRate));
    }

    if (InstanceData->StartingSection != NAME_None)
    {
        Parts.Add(FString::Printf(TEXT("Section '%s'"), *InstanceData->StartingSection.ToString()));
    }
    else if (!FMath::IsNearlyZero(InstanceData->StartingPosition))
    {
        Parts.Add(FString::Printf(TEXT("Position %.2f"), InstanceData->StartingPosition));
    }

    if (InstanceData->bShouldStopAllMontages)
    {
        Parts.Add(TEXT("StopAll"));
    }

    const FString Desc = FString::Join(Parts, TEXT(" | "));

	if (Formatting == EStateTreeNodeFormatting::RichText)
	{
		return FText::FromString(FString::Printf(TEXT("<b>%s</>"), *Desc));
	}
	return FText::FromString(Desc);
}
#endif

#undef LOCTEXT_NAMESPACE



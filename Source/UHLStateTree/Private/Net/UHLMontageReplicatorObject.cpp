// Pavel Penkov 2025 All Rights Reserved.

#include "Net/UHLMontageReplicatorObject.h"
#include "Animation/AnimInstance.h"
#include "Engine/ActorChannel.h"

void UUHLMontageReplicatorObject::Initialize(AActor* InOwner)
{
	Owner = InOwner;
}

int32 UUHLMontageReplicatorObject::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	return Owner ? Owner->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local;
}

bool UUHLMontageReplicatorObject::CallRemoteFunction(UFunction* Function, void* Params, FOutParmRec* OutParms, FFrame* Stack)
{
	if (Owner)
	{
		UNetDriver* NetDriver = Owner->GetNetDriver();
		if (NetDriver)
		{
			NetDriver->ProcessRemoteFunction(Owner, Function, Params, OutParms, Stack, this);
			return true;
		}
	}
	return false;
}

void UUHLMontageReplicatorObject::Multicast_PlayMontage_Implementation(
	USkeletalMeshComponent* Mesh,
	UAnimMontage* Montage,
	float PlayRate,
	float StartPosition,
	FName StartSection)
{
	if (!Mesh || !Montage) return;
	if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
	{
		AnimInstance->Montage_Play(Montage, PlayRate, EMontagePlayReturnType::MontageLength, StartSection != NAME_None ? 0.0f : StartPosition, true);
		if (StartSection != NAME_None)
		{
			AnimInstance->Montage_JumpToSection(StartSection, Montage);
		}
	}
}

void UUHLMontageReplicatorObject::Multicast_StopAllMontages_Implementation(USkeletalMeshComponent* Mesh, float BlendOutTime)
{
	if (!Mesh) return;
	if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
	{
		AnimInstance->StopAllMontages(BlendOutTime);
	}
}



// Pavel Penkov 2025 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "UHLMontageReplicatorObject.generated.h"

UCLASS()
class UHLSTATETREE_API UUHLMontageReplicatorObject : public UObject
{
	GENERATED_BODY()

public:
	UUHLMontageReplicatorObject() {}

	// Required so the UObject can take part in networking
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Params, FOutParmRec* OutParms, FFrame* Stack) override;
	virtual UWorld* GetWorld() const override { return Owner ? Owner->GetWorld() : nullptr; }

	void Initialize(AActor* InOwner);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayMontage(
		USkeletalMeshComponent* Mesh,
		UAnimMontage* Montage,
		float PlayRate,
		float StartPosition,
		FName StartSection);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StopAllMontages(USkeletalMeshComponent* Mesh, float BlendOutTime);

private:
	UPROPERTY()
	TObjectPtr<AActor> Owner = nullptr;
};



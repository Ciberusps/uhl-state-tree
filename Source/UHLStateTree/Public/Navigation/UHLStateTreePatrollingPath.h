// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UHLStateTreePatrollingPath.generated.h"

class USplineComponent;

UCLASS()
class UHLSTATETREE_API AUHLStateTreePatrollingPath : public AActor
{
    GENERATED_BODY()

public:
    AUHLStateTreePatrollingPath();

    const TArray<FVector>& GetWaypoints() const;

protected:
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Path")
    TObjectPtr<USplineComponent> Spline;

private:
    TArray<FVector>* Waypoints = new TArray<FVector>();

};

// Fill out your copyright notice in the Description page of Project Settings.


#include "Navigation/UHLStateTreePatrollingPath.h"

#include "Components/SplineComponent.h"

AUHLStateTreePatrollingPath::AUHLStateTreePatrollingPath()
{
    Spline = CreateDefaultSubobject<USplineComponent>("Spline");
    Spline->SetupAttachment(RootComponent);
    Spline->SetClosedLoop(true);
    for (int32 i = 0; i <= Spline->GetNumberOfSplinePoints(); i++)
    {
        Spline->SetSplinePointType(i, ESplinePointType::Linear);
    }
}

const TArray<FVector>& AUHLStateTreePatrollingPath::GetWaypoints() const
{
    Waypoints->Empty();

    for (int32 i = 0; i < Spline->GetNumberOfSplinePoints(); i++)
    {
        Waypoints->Add(Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local));
    }
    return *Waypoints;
}

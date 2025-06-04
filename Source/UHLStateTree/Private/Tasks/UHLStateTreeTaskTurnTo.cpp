// Pavel Penkov 2025 All Rights Reserved.

#include "Tasks/UHLStateTreeTaskTurnTo.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "Core/UHLAIActorSettings.h"
#include "Tasks/BTT_TurnTo.h"
#include "GameFramework/Character.h"
#include "Utils/UnrealHelperLibraryBPL.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UHLStateTreeTaskTurnTo)

#define LOCTEXT_NAMESPACE "UHLStateTreeTaskTurnTo"

EStateTreeRunStatus FUHLStateTreeTaskTurnTo::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	EStateTreeRunStatus Result = InstanceData.bInfinite
									? EStateTreeRunStatus::Running
									: EStateTreeRunStatus::Failed;
	
	const UWorld* World = Context.GetWorld();
	// if (World == nullptr && InstanceData.ReferenceActor != nullptr)
	// {
	// 	World = InstanceData.ReferenceActor->GetWorld();
	// }

	// Reference actor is not required (offset will be used as a global world location)
	// but a valid world is required.
	if (World == nullptr)
	{
		return InstanceData.bInfinite
				? EStateTreeRunStatus::Running
				: EStateTreeRunStatus::Failed;
	}

	AAIController* AIController = InstanceData.AIController;
	if (!AIController) return InstanceData.bInfinite
						? EStateTreeRunStatus::Running
						: EStateTreeRunStatus::Failed;

	// GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("[UHLStateTreeTaskTurnTo] using UUHLStateTreeAIComponent required to use SetCooldownTask"));

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn) return InstanceData.bInfinite
						? EStateTreeRunStatus::Running
						: EStateTreeRunStatus::Failed;

	const FVector PawnLocation = Pawn->GetActorLocation();
	InstanceData.PrecisionDot = FMath::Cos(FMath::DegreesToRadians(InstanceData.Precision));

	if (InstanceData.TargetActor)
	{
		AActor* ActorValue = InstanceData.TargetActor;

		if (ActorValue != NULL)
		{
			const FVector::FReal AngleDifference = TurnToStatics::CalculateAngleDifferenceDot(Pawn->GetActorForwardVector()
				, (ActorValue->GetActorLocation() - PawnLocation));

			if (AngleDifference >= InstanceData.PrecisionDot)
			{
				Result = InstanceData.bInfinite
						? EStateTreeRunStatus::Running
						: EStateTreeRunStatus::Succeeded;
			}
			else
			{
				AIController->SetFocus(ActorValue, EAIFocusPriority::Gameplay);
			    if (Pawn->GetClass()->ImplementsInterface(UUHLAIActorSettings::StaticClass()))
			    {
			        InstanceData.CurrentTurnSettings = GetTurnSettings(Context, Pawn);
			    }
				Result = EStateTreeRunStatus::Running;
			}
		}
	}
    // TODO add support for Vectors/Rotators
	else
	{
		if (FAISystem::IsValidLocation(InstanceData.TargetLocation))
		{
			const FVector::FReal AngleDifference = TurnToStatics::CalculateAngleDifferenceDot(Pawn->GetActorForwardVector()
				, (InstanceData.TargetLocation - PawnLocation));
	
			if (AngleDifference >= InstanceData.PrecisionDot)
			{
				Result = InstanceData.bInfinite
						? EStateTreeRunStatus::Running
						: EStateTreeRunStatus::Succeeded;
			}
			else
			{
				AIController->SetFocalPoint(InstanceData.TargetLocation, EAIFocusPriority::Gameplay);
				if (Pawn->GetClass()->ImplementsInterface(UUHLAIActorSettings::StaticClass()))
				{
					InstanceData.CurrentTurnSettings = GetTurnSettings(Context, Pawn);
				}
				Result = EStateTreeRunStatus::Running;
			}
		}
	}
	// else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Rotator::StaticClass())
	// {
	// 	const FRotator KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Rotator>(BlackboardKey.GetSelectedKeyID());
	//
	// 	if (FAISystem::IsValidRotation(KeyValue))
	// 	{
	// 		const FVector DirectionVector = KeyValue.Vector();
	// 		const FVector::FReal AngleDifference = CalculateAngleDifferenceDot(Pawn->GetActorForwardVector(), DirectionVector);
	//
	// 		if (AngleDifference >= PrecisionDot)
	// 		{
	// 			Result = EBTNodeResult::Succeeded;
	// 		}
	// 		else
	// 		{
	// 			const FVector FocalPoint = PawnLocation + DirectionVector * 10000.0f;
	// 			// set focal somewhere far in the indicated direction
	// 			AIController->SetFocalPoint(FocalPoint, EAIFocusPriority::Gameplay);
	// 			MyMemory->FocusLocationSet = FocalPoint;
	// 			Result = EBTNodeResult::InProgress;
	// 		}
	// 	}
	// }

	return Result;
}

EStateTreeRunStatus FUHLStateTreeTaskTurnTo::Tick(
	FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	const UWorld* World = Context.GetWorld();
	// if (World == nullptr && InstanceData.ReferenceActor != nullptr)
	// {
	// 	World = InstanceData.ReferenceActor->GetWorld();
	// }

	// Reference actor is not required (offset will be used as a global world location)
	// but a valid world is required.
	if (World == nullptr)
	{
		return InstanceData.bInfinite
				? EStateTreeRunStatus::Running
				: EStateTreeRunStatus::Failed;
	}

	AAIController* AIController = InstanceData.AIController;
	if (!AIController || !AIController->GetPawn())
	{
		return InstanceData.bInfinite
				? EStateTreeRunStatus::Running
				: EStateTreeRunStatus::Failed;
	}
	
	// target enemy if its infinite task
	if (InstanceData.bInfinite
		&& InstanceData.TargetActor
		&& AIController->GetFocusActorForPriority(EAIFocusPriority::Gameplay) != InstanceData.TargetActor)
	{
		AIController->SetFocus(InstanceData.TargetActor, EAIFocusPriority::Gameplay);
	}
	const FVector PawnDirection = AIController->GetPawn()->GetActorForwardVector();
   	const FVector FocalPoint = AIController->GetFocalPointForPriority(EAIFocusPriority::Gameplay);
    ACharacter* AICharacter = AIController->GetCharacter();

	if (FocalPoint != FAISystem::InvalidLocation)
	{
	    float DeltaAngleRad = TurnToStatics::CalculateAngleDifferenceDot(PawnDirection, FocalPoint - AIController->GetPawn()->GetActorLocation());
	    // float DeltaAngle = FMath::RadiansToDegrees(FMath::Acos(DeltaAngleRad));
	    float DeltaAngle = InstanceData.TargetActor
			? UUnrealHelperLibraryBPL::RelativeAngleToActor(AICharacter, InstanceData.TargetActor)
			: UUnrealHelperLibraryBPL::RelativeAngleToVector(AICharacter, InstanceData.TargetLocation);

		if (InstanceData.bDebug)
		{
			UUnrealHelperLibraryBPL::DebugPrintStrings(FString::Printf(TEXT("DeltaAngle %f"), DeltaAngle), "", "", "", "", "", "", "", "", "", -1, FName("Test"));
			FVector CurrentLocation = InstanceData.TargetActor
				? InstanceData.TargetActor->GetActorLocation()
				: InstanceData.TargetLocation;
			DrawDebugSphere(AIController->GetWorld(), CurrentLocation,
				50.0f, 12, FColor::Blue, false, -1);	
		}

		if (DeltaAngleRad >= InstanceData.PrecisionDot)
		{
			if (InstanceData.bDebug)
			{
				UUnrealHelperLibraryBPL::DebugPrintStrings(FString::Printf(TEXT("TurnRange->bOverrideStopMontageOnGoalReached %hhd"), InstanceData.CurrentTurnRange.bOverrideStopMontageOnGoalReached));
			}
		    bool bCanStopMontage = false;
		    if (InstanceData.CurrentTurnRange.bOverrideStopMontageOnGoalReached)
		    {
		        bCanStopMontage = InstanceData.CurrentTurnRange.bStopMontageOnGoalReached;
		    }
		    else
		    {
		        bCanStopMontage = InstanceData.CurrentTurnSettings.bStopMontageOnGoalReached;
		    }

		    if (bCanStopMontage)
		    {
		        AICharacter->StopAnimMontage();
			    AIController->ClearFocus(EAIFocusPriority::Gameplay);
		        // CleanUp(*AIController, NodeMemory);
			    return InstanceData.bInfinite
			    	? EStateTreeRunStatus::Running
			    	: EStateTreeRunStatus::Succeeded;
		    }
		    else
		    {
			    AIController->ClearFocus(EAIFocusPriority::Gameplay);
		        // CleanUp(*AIController, NodeMemory);
			    return InstanceData.bInfinite
					? EStateTreeRunStatus::Running
					: EStateTreeRunStatus::Succeeded;
		    }
		}
	    else
	    {
	        if (TurnToStatics::IsTurnWithAnimationRequired(AICharacter))
	        {
	        	// if (AIController->GetFocusActorForPriority(EAIFocusPriority::Gameplay) != InstanceData.TargetActor)
	        	// {
	        	// 	AIController->SetFocus(InstanceData.TargetActor, EAIFocusPriority::Gameplay);
	        	// }
	        	
		        bool bCurrentTurnRangeSet = false;
	            InstanceData.CurrentTurnRange = TurnToStatics::GetTurnRange(DeltaAngle, bCurrentTurnRangeSet, InstanceData.CurrentTurnSettings);
	            if (bCurrentTurnRangeSet && InstanceData.CurrentTurnRange.AnimMontage)
	            {
	                AICharacter->PlayAnimMontage(InstanceData.CurrentTurnRange.AnimMontage);
	            }

	            // TODO тут ошибка?
	            // finish if no turn animation found and "bTurnOnlyWithAnims"
	            if (!bCurrentTurnRangeSet && InstanceData.CurrentTurnSettings.bTurnOnlyWithAnims)
	            {
		            AIController->ClearFocus(EAIFocusPriority::Gameplay);
	                // CleanUp(*AIController, NodeMemory);
		            return InstanceData.bInfinite
						? EStateTreeRunStatus::Running
						: EStateTreeRunStatus::Succeeded;
	            }
	        }
	    }
	}
	else
	{
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
		// CleanUp(*AIController, NodeMemory);
		return InstanceData.bInfinite
					? EStateTreeRunStatus::Running
					: EStateTreeRunStatus::Failed;
	}
	
	return FStateTreeTaskCommonBase::Tick(Context, DeltaTime);
}

#if WITH_EDITOR
FText FUHLStateTreeTaskTurnTo::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	const FText Format = (Formatting == EStateTreeNodeFormatting::RichText)
		? LOCTEXT("DebugTextRich", "<b>Turn To</> \"{Text}\"")
		: LOCTEXT("DebugText", "Turn To \"{Text}\"");

	return FText::FormatNamed(Format, TEXT("Text"),
		FText::FromString(InstanceData->TargetActor
			? InstanceData->TargetActor.GetFullName()
			: InstanceData->TargetLocation.ToString()));
}
#endif

FTurnSettings FUHLStateTreeTaskTurnTo::GetTurnSettings(FStateTreeExecutionContext& Context, AActor* Actor) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	FTurnSettings Result;
	if (InstanceData.SettingsSource == EUHLSettingsSource::Actor)
	{
		Result = IUHLAIActorSettings::Execute_GetTurnSettings(Actor);
	}
	if (InstanceData.SettingsSource == EUHLSettingsSource::DataAsset)
	{
		Result = InstanceData.RotateToAnimationsDataAsset->TurnSettings;
	}
	if (InstanceData.SettingsSource == EUHLSettingsSource::Node)
	{
		Result = InstanceData.TurnSettings;
	}
	return Result;
}

#undef LOCTEXT_NAMESPACE
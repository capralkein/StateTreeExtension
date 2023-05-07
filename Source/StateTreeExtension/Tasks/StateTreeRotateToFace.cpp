// Copyright CapralKein. All rights reserved.

#include "StateTreeRotateToFace.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/AITask_MoveTo.h"

FStateTreeRotateToFace::FStateTreeRotateToFace()
{
	Precision = 10.f;
}

namespace
{
	FORCEINLINE_DEBUGGABLE float CalculateAngleDifferenceDot(const FVector& VectorA, const FVector& VectorB)
	{
		return (VectorA.IsNearlyZero() || VectorB.IsNearlyZero())
			? 1.f
			: VectorA.CosineAngle2D(VectorB);
	}
}

EStateTreeRunStatus FStateTreeRotateToFace::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	InstanceData.PrecisionDot = FMath::Cos(FMath::DegreesToRadians(Precision));
	InstanceData.bFinished = false;
	InstanceData.FocusActor = nullptr;
	InstanceData.FocusLocation = FAISystem::InvalidLocation;

	return PerformRotateTask(Context, InstanceData);
}

EStateTreeRunStatus FStateTreeRotateToFace::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	if (InstanceData.bFinished)
		return EStateTreeRunStatus::Succeeded;
	
	const AAIController* Controller = InstanceData.GetAIController();

	if (!IsValid(Controller) || !IsValid(Controller->GetPawn()))
		return EStateTreeRunStatus::Failed;
	
	const FVector PawnDirection = Controller->GetPawn()->GetActorForwardVector();				
	const FVector FocalPoint = Controller->GetFocalPointForPriority(EAIFocusPriority::Gameplay);

	if (!FAISystem::IsValidLocation(FocalPoint))
	{
		return EStateTreeRunStatus::Failed;	
	}

	if (CalculateAngleDifferenceDot(PawnDirection, FocalPoint - Controller->GetPawn()->GetActorLocation()) >= InstanceData.PrecisionDot)
		return EStateTreeRunStatus::Succeeded;

	return EStateTreeRunStatus::Running;
}

void FStateTreeRotateToFace::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	CleanUp(InstanceData);
}

EStateTreeRunStatus FStateTreeRotateToFace::PerformRotateTask(const FStateTreeExecutionContext& Context, FStateTreeRotateToFaceInstanceData& InstanceData) const
{
	AAIController* Controller = InstanceData.GetAIController();

	if (!IsValid(Controller) || !IsValid(Controller->GetPawn()))
		return EStateTreeRunStatus::Failed;

	const APawn* Pawn = Controller->GetPawn();
	const FVector PawnLocation = Pawn->GetActorLocation();

	FVector TargetLocation = FAISystem::InvalidLocation;
	
	if (IsValid(InstanceData.GoalActor))
	{
		TargetLocation = InstanceData.GoalActor->GetActorLocation();
	}
	else
	{
		TargetLocation = InstanceData.GoalLocation;
	}

	if (!FAISystem::IsValidLocation(TargetLocation))
	{
		return EStateTreeRunStatus::Failed;
	}
	
	const float AngleDifference = CalculateAngleDifferenceDot(Pawn->GetActorForwardVector()
			, (TargetLocation - PawnLocation));
	
	if (AngleDifference >= InstanceData.PrecisionDot)
	{
		InstanceData.bFinished = true;
		return EStateTreeRunStatus::Succeeded;
	}

	if (IsValid(InstanceData.GoalActor))
	{
		InstanceData.FocusActor = InstanceData.GoalActor;
		Controller->SetFocus(InstanceData.GoalActor, EAIFocusPriority::Gameplay);
	}
	else
	{
		InstanceData.FocusLocation = InstanceData.GoalLocation;
		Controller->SetFocalPoint(TargetLocation, EAIFocusPriority::Gameplay);
	}
	
	return EStateTreeRunStatus::Running;
}

void FStateTreeRotateToFace::CleanUp(FInstanceDataType& InstanceData) const
{
	AAIController* Controller = InstanceData.GetAIController();
	
	if (IsValid(Controller))
	{
		bool bClearFocus = false;

		if (IsValid(InstanceData.FocusActor))
		{
			bClearFocus = InstanceData.FocusActor == Controller->GetFocusActorForPriority(EAIFocusPriority::Gameplay);
		}
		else if (InstanceData.FocusLocation != FAISystem::InvalidLocation)
		{
			bClearFocus = InstanceData.FocusLocation == Controller->GetFocalPointForPriority(EAIFocusPriority::Gameplay);
		}

		if(bClearFocus)
		{
			Controller->ClearFocus(EAIFocusPriority::Gameplay);
		}
	}
	
	InstanceData.bFinished = false;
	InstanceData.GoalActor = nullptr;
	InstanceData.GoalLocation = FAISystem::InvalidLocation;
	InstanceData.FocusActor = nullptr;
	InstanceData.FocusLocation = FAISystem::InvalidLocation;
}

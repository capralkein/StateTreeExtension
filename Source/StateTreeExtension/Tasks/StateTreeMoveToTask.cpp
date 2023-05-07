// Copyright CapralKein. All rights reserved.

#include "StateTreeMoveToTask.h"
#include "AISystem.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/AITask_MoveTo.h"
#include "VisualLogger/VisualLogger.h"

FStateTreeMoveToTask::FStateTreeMoveToTask()
{
	bUseActorForGoal = false;
	AcceptableRadius = GET_AI_CONFIG_VAR(AcceptanceRadius);
	ObservedLocationTolerance = AcceptableRadius * 0.95f;
	bReachTestIncludesGoalRadius = bReachTestIncludesAgentRadius = GET_AI_CONFIG_VAR(bFinishMoveOnGoalOverlap);
	bAllowStrafe = GET_AI_CONFIG_VAR(bAllowStrafing);
	bAllowPartialPath = GET_AI_CONFIG_VAR(bAcceptPartialPaths);
	bTrackMovingGoal = true;
	bProjectGoalLocation = true;
	bObserveGoal = true;
	bUsePathfinding = true;
}

EStateTreeRunStatus FStateTreeMoveToTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	InstanceData.PreviousGoalLocation = FAISystem::InvalidLocation;
	InstanceData.MoveRequestId = FAIRequestID::InvalidRequest;

	const AAIController* Controller = InstanceData.GetAIController();
	if (!IsValid(Controller))
	{
		UE_VLOG(Context.GetOwner(), LogStateTree, Warning, TEXT("%s failed as the AIController is missing"), *Name.ToString());
		return EStateTreeRunStatus::Failed;
	}

	return PerformMoveTask(Context, InstanceData);
}

EStateTreeRunStatus FStateTreeMoveToTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	if (!InstanceData.MoveTask)
	{
		UE_VLOG(Context.GetOwner(), LogStateTree, Warning, TEXT("%s's ai task is null, failing task"), *Name.ToString());
		return EStateTreeRunStatus::Failed;
	}
	
	if (InstanceData.MoveTask->GetState() == EGameplayTaskState::Finished)
	{
		if (InstanceData.MoveTask->WasMoveSuccessful())
		{
			UE_VLOG(Context.GetOwner(), LogStateTree, Verbose, TEXT("%s succeeded"), *Name.ToString());
			return EStateTreeRunStatus::Succeeded;
		}
		else
		{
			UE_VLOG(Context.GetOwner(), LogStateTree, Verbose, TEXT("%s failed"), *Name.ToString());
			return EStateTreeRunStatus::Failed;
		}
	}

	if (!IsValid(InstanceData.MoveTask))
	{
		UE_VLOG(Context.GetOwner(), LogStateTree, Warning,
			TEXT("%s's ai task is invalid but hasn't been marked as finished, failing task"), *Name.ToString());
		return EStateTreeRunStatus::Failed;
	}

	if (bObserveGoal)
	{
		if (bUseActorForGoal)
		{
			if (InstanceData.MoveTask->GetMoveRequestRef().GetGoalActor() != InstanceData.GoalActor)
			{
				return PerformMoveTask(Context, InstanceData);
			}
		}
		else
		{
			if (InstanceData.GoalLocation != InstanceData.PreviousGoalLocation &&
				FVector::DistSquared(InstanceData.GoalLocation, InstanceData.PreviousGoalLocation) > FMath::Square(
					ObservedLocationTolerance))
			{
				return PerformMoveTask(Context, InstanceData);
			}
		}
	}

	return EStateTreeRunStatus::Running;
}

void FStateTreeMoveToTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	if (IsValid(InstanceData.MoveTask))
	{
		InstanceData.MoveTask->ExternalCancel();
	}

	InstanceData.MoveTask = nullptr;
}

EStateTreeRunStatus FStateTreeMoveToTask::PerformMoveTask(const FStateTreeExecutionContext& Context, FStateTreeMoveToTaskInstanceData& InstanceData) const
{
	AAIController* MyController = InstanceData.GetAIController();
	if (!IsValid(MyController))
		return EStateTreeRunStatus::Failed;

	FAIMoveRequest MoveReq;
	MoveReq.SetNavigationFilter(*FilterClass ? FilterClass : MyController->GetDefaultNavigationFilterClass());
	MoveReq.SetAllowPartialPath(bAllowPartialPath);
	MoveReq.SetAcceptanceRadius(AcceptableRadius);
	MoveReq.SetCanStrafe(bAllowStrafe);
	MoveReq.SetReachTestIncludesAgentRadius(bReachTestIncludesAgentRadius);
	MoveReq.SetReachTestIncludesGoalRadius(bReachTestIncludesGoalRadius);
	MoveReq.SetProjectGoalLocation(bProjectGoalLocation);
	MoveReq.SetUsePathfinding(bUsePathfinding);

	if (bUseActorForGoal)
	{
		if (!IsValid(InstanceData.GoalActor))
		{
			UE_VLOG(Context.GetOwner(), LogStateTree, Warning, TEXT("%s failed as bUseActorForGoal was true but no goal actor was specified"), *Name.ToString());
			return EStateTreeRunStatus::Failed;
		}

		if (bTrackMovingGoal)
		{
			MoveReq.SetGoalActor(InstanceData.GoalActor);
		}
		else
		{
			MoveReq.SetGoalLocation(InstanceData.GoalActor->GetActorLocation());
		}
	}
	else
	{
		MoveReq.SetGoalLocation(InstanceData.GoalLocation);
		InstanceData.PreviousGoalLocation = InstanceData.GoalLocation;
	}

	if (!MoveReq.IsValid()) return EStateTreeRunStatus::Failed;

	UAITask_MoveTo* MoveTask = InstanceData.MoveTask.Get();
	const bool bReuseExistingTask = MoveTask != nullptr;

	MoveTask = PrepareMoveTask(*MyController, MoveTask, MoveReq);
	if (!MoveTask) return EStateTreeRunStatus::Failed;

	if (bReuseExistingTask)
	{
		if (MoveTask->IsActive())
		{
			MoveTask->ConditionalPerformMove();
		}
	}
	else
	{
		InstanceData.MoveTask = MoveTask;
		MoveTask->ReadyForActivation();
	}

	return (MoveTask->GetState() != EGameplayTaskState::Finished) ?	EStateTreeRunStatus::Running :
		MoveTask->WasMoveSuccessful() ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Failed;
}

UAITask_MoveTo* FStateTreeMoveToTask::PrepareMoveTask(AAIController& AIController, UAITask_MoveTo* ExistingTask, FAIMoveRequest& MoveReq) const
{
	UAITask_MoveTo* MoveTask = ExistingTask ? ExistingTask : UAITask::NewAITask<UAITask_MoveTo>(AIController, AIController, TEXT("StateTreeTask"));
	if (MoveTask)
	{
		MoveTask->SetUp(&AIController, MoveReq);
	}

	return MoveTask;
}

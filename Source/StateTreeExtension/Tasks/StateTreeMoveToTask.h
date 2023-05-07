// Copyright CapralKein. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "StateTreeTaskBase.h"
#include "StateTreeMoveToTask.generated.h"

class UNavigationQueryFilter;
class UAITask_MoveTo;

USTRUCT()
struct STATETREEEXTENSION_API FStateTreeMoveToTaskInstanceData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AActor> Actor = nullptr;

	UPROPERTY(EditAnywhere, Category = Input, meta = (Optional))
	TObjectPtr<AActor> GoalActor = nullptr;

	UPROPERTY(EditAnywhere, Category = Input, meta = (Optional))
	FVector GoalLocation = FVector::ZeroVector;
	
	FVector PreviousGoalLocation = FAISystem::InvalidLocation;
	FAIRequestID MoveRequestId = FAIRequestID::InvalidRequest;

	UPROPERTY()
	TObjectPtr<UAITask_MoveTo> MoveTask = nullptr;
public:
	AAIController* GetAIController() const
	{
		if (!IsValid(Actor)) return nullptr;
		
		if (AAIController* Controller = Cast<AAIController>(Actor)) return Controller;

		if (const APawn* Pawn = Cast<APawn>(Actor))	return Cast<AAIController>(Pawn->GetController());
		
		return nullptr;
	}
};

USTRUCT(meta = (DisplayName = "Move to"))
struct STATETREEEXTENSION_API FStateTreeMoveToTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
public:
	using FInstanceDataType = FStateTreeMoveToTaskInstanceData;
	
	FStateTreeMoveToTask();
	
	// FStateTreeNodeBase
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	//~FStateTreeNodeBase

	// FStateTreeTaskBase
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	//~FStateTreeTaskBase

private:
	EStateTreeRunStatus PerformMoveTask(const FStateTreeExecutionContext& Context, FStateTreeMoveToTaskInstanceData& InstanceData) const;
	UAITask_MoveTo* PrepareMoveTask(class AAIController& AIController, UAITask_MoveTo* ExistingTask, FAIMoveRequest& MoveReq) const;
	
public:
	/** Fixed distance added to threshold between AI and goal location in destination reach test */
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float AcceptableRadius;

	/** "None" will result in default filter being used */
	UPROPERTY(EditAnywhere, Category = "Settings")
	TSubclassOf<UNavigationQueryFilter> FilterClass;

	/** If task is expected to react to changes to location
	 *	this property can be used to tweak sensitivity of the mechanism. Value is 
	 *	recommended to be less than AcceptableRadius */
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = "1", UIMin = "1", EditCondition = "bObserveLocation", DisplayAfter = "bObserveLocation"))
	float ObservedLocationTolerance;

	// TODO
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (DisplayAfter = "Actor"))
	uint8 bUseActorForGoal : 1;

	/** If move goal changes the move will be redirected to new location */
	UPROPERTY(EditAnywhere, Category = "Settings")
	uint8 bObserveGoal : 1;

	UPROPERTY(EditAnywhere, Category = "Settings")
	uint8 bAllowStrafe : 1;

	/** If set, move will use pathfinding */
	UPROPERTY(EditAnywhere, Category = "Settings")
	uint8 bUsePathfinding : 1;

	/** If set, use incomplete path when goal can't be reached */
	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
	uint8 bAllowPartialPath : 1;

	/** If set, path to goal actor will update itself when actor moves */
	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
	uint8 bTrackMovingGoal : 1;

	/** If set, goal location will be projected on navigation data (navmesh) before using */
	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
	uint8 bProjectGoalLocation : 1;

	/** If set, radius of AI's capsule will be added to threshold between AI and goal location in destination reach test  */
	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
	uint8 bReachTestIncludesAgentRadius : 1;

	/** If set, radius of goal's capsule will be added to threshold between AI and goal location in destination reach test  */
	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
	uint8 bReachTestIncludesGoalRadius : 1;
};

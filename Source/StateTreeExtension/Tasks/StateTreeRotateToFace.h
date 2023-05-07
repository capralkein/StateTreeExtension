// Copyright CapralKein. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "StateTreeTaskBase.h"
#include "StateTreeRotateToFace.generated.h"

class UNavigationQueryFilter;
class UAITask_MoveTo;

USTRUCT()
struct STATETREEEXTENSION_API FStateTreeRotateToFaceInstanceData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AActor> Actor = nullptr;

	UPROPERTY(EditAnywhere, Category = Input, meta = (Optional))
	TObjectPtr<AActor> GoalActor = nullptr;

	UPROPERTY(EditAnywhere, Category = Input, meta = (Optional))
	FVector GoalLocation = FAISystem::InvalidLocation;

	UPROPERTY()
	TObjectPtr<AActor> FocusActor = nullptr;

	FVector FocusLocation = FAISystem::InvalidLocation;

	float PrecisionDot;

	uint8 bFinished : 1;
	
public:
	AAIController* GetAIController() const
	{
		if (!IsValid(Actor)) return nullptr;
		
		if (AAIController* Controller = Cast<AAIController>(Actor)) return Controller;

		if (const APawn* Pawn = Cast<APawn>(Actor))	return Cast<AAIController>(Pawn->GetController());
		
		return nullptr;
	}
};

USTRUCT(meta = (DisplayName = "Rotate to face"))
struct STATETREEEXTENSION_API FStateTreeRotateToFace : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
public:
	using FInstanceDataType = FStateTreeRotateToFaceInstanceData;
	
	FStateTreeRotateToFace();
	
	// FStateTreeNodeBase
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	//~FStateTreeNodeBase

	// FStateTreeTaskBase
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	//~FStateTreeTaskBase

private:
	EStateTreeRunStatus PerformRotateTask(const FStateTreeExecutionContext& Context, FStateTreeRotateToFaceInstanceData& InstanceData) const;
	void CleanUp(FInstanceDataType& InstanceData) const;
public:
	/** -- */
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float Precision;
};

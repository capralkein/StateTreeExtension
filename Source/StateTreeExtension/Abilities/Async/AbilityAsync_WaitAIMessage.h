// Copyright CapralKein. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BrainComponent.h"
#include "Abilities/Async/AbilityAsync.h"
#include "AbilityAsync_WaitAIMessage.generated.h"

class UBrainComponent;
struct FAIMessage;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityAsyncMessageReceive, UObject*, Sender);

UCLASS()
class STATETREEEXTENSION_API UAbilityAsync_WaitAIMessage : public UCancellableAsyncAction
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "AI|Async", meta = (DefaultToSelf = "TargetActor", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityAsync_WaitAIMessage* WaitForAIMessage(AActor* TargetActor, FName InMessage, bool TriggerOnce=true);

	UPROPERTY(BlueprintAssignable)
	FAbilityAsyncMessageReceive OnReceive;

protected:
	// UCancellableAsyncAction
	virtual void Activate() override;
	virtual void Cancel() override;
	//~UCancellableAsyncAction
	
	void OnMessageReceive(UBrainComponent* BrainComponent, const FAIMessage& Message);

	FAIMessageObserverHandle AIMessageObserverHandle;

private:
	FName Message = NAME_None;
	bool TriggerOnce = false;
	UPROPERTY()
	UBrainComponent* BrainComponent = nullptr;
};

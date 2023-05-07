// Copyright CapralKein. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Async/AbilityAsync.h"

#include "AbilityAsync_WaitAbilityEnd.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityAsyncAbilityEndDelegate, UGameplayAbility*, ActivatedAbility);

UCLASS()
class STATETREEEXTENSION_API UAbilityAsync_WaitAbilityEnd : public UAbilityAsync
{
	GENERATED_BODY()

public:
	/**
	 * Wait until a GameplayEffect is applied to a target actor
	 * If TriggerOnce is true, this action will only activate one time. Otherwise it will return every time a GA is activate
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Async", meta = (DefaultToSelf = "TargetActor", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityAsync_WaitAbilityEnd* WaitForAbilityEnd(AActor* TargetActor, const FGameplayAbilitySpecHandle& AbilitySpecHandle, bool TriggerOnce=true);

	UPROPERTY(BlueprintAssignable)
	FAbilityAsyncAbilityEndDelegate OnEnded;
	
protected:
	// UAbilityAsync
	virtual void Activate() override;
	virtual void EndAction() override;
	//~UAbilityAsync

protected:
	UFUNCTION()
	void OnAbilityEnd(UGameplayAbility* GameplayAbility);
protected:
	FGameplayAbilitySpecHandle AbilitySpecHandle = FGameplayAbilitySpecHandle();
	bool TriggerOnce = false;

	FDelegateHandle OnAbilityEndDelegateHandle;
};

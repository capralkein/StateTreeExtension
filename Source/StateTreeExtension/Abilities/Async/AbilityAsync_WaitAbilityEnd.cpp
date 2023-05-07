// Copyright CapralKein. All rights reserved.

#include "AbilityAsync_WaitAbilityEnd.h"
#include "AbilitySystemComponent.h"

UAbilityAsync_WaitAbilityEnd* UAbilityAsync_WaitAbilityEnd::WaitForAbilityEnd(AActor* TargetActor, const FGameplayAbilitySpecHandle& InAbilitySpecHandle, bool InTriggerOnce)
{
	UAbilityAsync_WaitAbilityEnd* MyObj = NewObject<UAbilityAsync_WaitAbilityEnd>();
	MyObj->SetAbilityActor(TargetActor);
	MyObj->TriggerOnce = InTriggerOnce;
	MyObj->AbilitySpecHandle = InAbilitySpecHandle;
	return MyObj;
}

void UAbilityAsync_WaitAbilityEnd::Activate()
{
	Super::Activate();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	
	if(!IsValid(ASC))
	{
		EndAction();
		return;
	}
	
	OnAbilityEndDelegateHandle = ASC->AbilityEndedCallbacks.AddUObject(this, &UAbilityAsync_WaitAbilityEnd::OnAbilityEnd);
}

void UAbilityAsync_WaitAbilityEnd::EndAction()
{
	Super::EndAction();
}

void UAbilityAsync_WaitAbilityEnd::OnAbilityEnd(UGameplayAbility* GameplayAbility)
{
	if(AbilitySpecHandle != GameplayAbility->GetCurrentAbilitySpecHandle())
		return;
	
	if(ShouldBroadcastDelegates())
	{
		OnEnded.Broadcast(GameplayAbility);
	}
	
	if(TriggerOnce)
	{
		EndAction();
	}
}

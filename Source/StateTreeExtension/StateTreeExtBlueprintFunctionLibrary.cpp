// Copyright CapralKein. All rights reserved.

#include "StateTreeExtBlueprintFunctionLibrary.h"

#include "AbilitySystemComponent.h"

UActorComponent* UStateTreeExtBlueprintFunctionLibrary::FindComponentByClass(const AActor* Actor, TSubclassOf<UActorComponent> ComponentClass)
{
	if (!IsValid(Actor) || !IsValid(ComponentClass))
		return nullptr;

	UActorComponent* Component = Actor->GetComponentByClass(ComponentClass);
	if (IsValid(Component))
		return Component;

	if (const APawn* Pawn = Cast<APawn>(Actor))
	{
		if (const AController* Controller = Pawn->GetController())
		{
			Component = Controller->GetComponentByClass(ComponentClass);
			if (IsValid(Component))
				return Component;
		}
	}

	if (const AController* Controller = Cast<AController>(Actor))
	{
		if (const APawn* Pawn = Controller->GetPawn())
		{
			Component = Pawn->GetComponentByClass(ComponentClass);
			if (IsValid(Component))
				return Component;
		}
	}

	return nullptr;
}

bool UStateTreeExtBlueprintFunctionLibrary::CanActivateAbility(UAbilitySystemComponent* ASC,
	const FGameplayAbilitySpecHandle& AbilitySpecHandle)
{
	if (!AbilitySpecHandle.IsValid() || !IsValid(ASC))
		return false;

	const FGameplayAbilitySpec* AbilitySpecPtr = ASC->FindAbilitySpecFromHandle(AbilitySpecHandle);
	if (!AbilitySpecPtr)
		return false;

	const UGameplayAbility* CheckAbility = AbilitySpecPtr->GetPrimaryInstance();
	if (!CheckAbility)
		CheckAbility = AbilitySpecPtr->Ability;
	
	if (AbilitySpecPtr->IsActive())
		return false;
	
	if (ASC->IsAbilityInputBlocked(AbilitySpecPtr->InputID))
		return false;
	
	if (ASC->AreAbilityTagsBlocked(AbilitySpecPtr->Ability->AbilityTags))
		return false;

	if (FGameplayTagContainer FailureTags; !CheckAbility->CanActivateAbility(
		AbilitySpecPtr->Handle, ASC->AbilityActorInfo.Get(), nullptr, nullptr, &FailureTags))
	{
		return false;
	}

	return true;
}

// Copyright CapralKein. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StateTreeExtBlueprintFunctionLibrary.generated.h"

struct FGameplayAbilitySpecHandle;
class UAbilitySystemComponent;

UCLASS()
class STATETREEEXTENSION_API UStateTreeExtBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "StateTreeExtension", meta = (ComponentClass = "/Script/Engine.ActorComponent", DeterminesOutputType = "ComponentClass", KeyWords = "Find"))
	static UActorComponent* FindComponentByClass(const AActor* Actor, TSubclassOf<UActorComponent> ComponentClass);

	UFUNCTION(BlueprintPure, Category = "StateTreeExtension|AbilitySystem")
	static bool CanActivateAbility(UAbilitySystemComponent* AbilitySystem, const FGameplayAbilitySpecHandle& AbilitySpecHandle);
	
};

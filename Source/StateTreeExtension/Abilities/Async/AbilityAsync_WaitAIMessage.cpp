// Copyright CapralKein. All rights reserved.

#include "AbilityAsync_WaitAIMessage.h"

#include "BrainComponent.h"
#include "StateTreeExtension/StateTreeExtBlueprintFunctionLibrary.h"

UAbilityAsync_WaitAIMessage* UAbilityAsync_WaitAIMessage::WaitForAIMessage(AActor* TargetActor, FName InMessage, bool InTriggerOnce)
{
	UAbilityAsync_WaitAIMessage* MyObj = NewObject<UAbilityAsync_WaitAIMessage>();
	MyObj->BrainComponent = Cast<UBrainComponent>(UStateTreeExtBlueprintFunctionLibrary::FindComponentByClass(TargetActor, UBrainComponent::StaticClass()));
	if(!MyObj->BrainComponent)
	{
		return nullptr;
	}
	
	MyObj->Message = InMessage;
	MyObj->TriggerOnce = InTriggerOnce;
	return MyObj;
}

void UAbilityAsync_WaitAIMessage::Activate()
{
	Super::Activate();
	if(!BrainComponent)
	{
		Cancel();
		return;
	}
	AIMessageObserverHandle = FAIMessageObserver::Create(BrainComponent, Message, FOnAIMessage::CreateUObject(this, &UAbilityAsync_WaitAIMessage::OnMessageReceive));
}

void UAbilityAsync_WaitAIMessage::Cancel()
{
	Super::Cancel();
	AIMessageObserverHandle.Reset();
}

void UAbilityAsync_WaitAIMessage::OnMessageReceive(UBrainComponent* InBrainComponent, const FAIMessage& InMessage)
{
	OnReceive.Broadcast(InMessage.Sender.Get());
	if(TriggerOnce)
	{
		Cancel();
	}
}

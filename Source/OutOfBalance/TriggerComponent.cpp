// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerComponent.h"

UTriggerComponent::UTriggerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTriggerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (moverActor) {
		mover = moverActor->FindComponentByClass<UMover>();
		if (mover) {
			UE_LOG(LogTemp, Display, TEXT("Succesfully found the mover component!"))
		}else {
			UE_LOG(LogTemp, Display, TEXT("Failed to find mover component!"))
		}
	}
	else {
		UE_LOG(LogTemp, Display, TEXT("moverActor is nullptr"))
	}

	if (isPressurePlate) {
		OnComponentBeginOverlap.AddDynamic(this, &UTriggerComponent::OnOverlapBegin);
		OnComponentEndOverlap.AddDynamic(this, &UTriggerComponent::OnOverlapEnd);
	}
}

void UTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTriggerComponent::Trigger(bool newTriggerValue)
{
	isTriggered = newTriggerValue;
	if (mover) {
		mover->setShouldMove(isTriggered);
	}else {
		UE_LOG(LogTemp, Display, TEXT("%s dosen't have a mover to trigger!"), *GetOwner()->GetActorNameOrLabel());
	}
}

void UTriggerComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("PressurePlateActivator")) {
		activatorCount++;
		if (!isTriggered) {
			Trigger(true);
		}
	}
}

void UTriggerComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->ActorHasTag("PressurePlateActivator")) {
		activatorCount--;
		if (isTriggered && activatorCount == 0) {
			Trigger(false);
		}
	}
}

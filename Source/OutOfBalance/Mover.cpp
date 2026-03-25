// Fill out your copyright notice in the Description page of Project Settings.


#include "Mover.h"

#include "Math/UnrealMathUtility.h"

// Sets default values for this component's properties
UMover::UMover()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMover::BeginPlay()
{
	Super::BeginPlay();

	startLocation = GetOwner()->GetActorLocation();
	setShouldMove(false);
}


// Called every frame
void UMover::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector currentLocation = GetOwner()->GetActorLocation();

	reachedTarget = currentLocation.Equals(targetLocation);

	if (!reachedTarget) {
		float speed = moveOffSet.Length() / moveTime;

		FVector newLocation = FMath::VInterpConstantTo(currentLocation, targetLocation, DeltaTime, speed);

		GetOwner()->SetActorLocation(newLocation);
	}

	
}

bool UMover::getShouldMove()
{
	return shouldMove;
}

void UMover::setShouldMove(bool newShouldMove)
{
	shouldMove = newShouldMove;

	if (shouldMove) {
		targetLocation = startLocation + moveOffSet;
	}
	else {
		targetLocation = startLocation;
	}
}


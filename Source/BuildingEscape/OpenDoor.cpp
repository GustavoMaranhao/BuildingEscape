// Copyright Gustavo Maranhão 2017

#include "OpenDoor.h"
#include "GameFramework/Actor.h"


// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
}

void UOpenDoor::OpenDoor() {
	// Find the owning actor and rotation
	AActor* owner = GetOwner();
	FRotator ownerRotation = owner->GetActorRotation();

	// Alter the rotation 90 degrees towards the starting point
	//FRotator newRotation = ownerRotation.Add(0.0f, 60.0f, 0.0f);

	//Temporary fix for the polling method of opening the door
	FRotator newRotation = FRotator(0.f, -60.f, 0.f);

	// Apply the new rotation to the actor
	owner->SetActorRotation(newRotation);
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Poll the Trigger Volume
	if (PressurePlate && PressurePlate->IsOverlappingActor(ActorThatOpens)) {
		//If the ActorThatOpens is in the volume
		OpenDoor();
	}	
}


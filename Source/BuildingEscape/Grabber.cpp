// Copyright Gustavo Maranhão 2017

#include "Grabber.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/InputComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

//This compiler diretiv does nothing, it simply is a way to remind ourselves that the parameters might be modified in the function
#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
}

///Look for attached physics handle
void UGrabber::FindPhysicsHandleComponent() {
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle) {
		//PhysicsHandle is found
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("PhysicsHandle not found in %s's components!"), *(GetOwner()->GetName()))
	}
}

///Look for the attached Input component (only appears at runtime)
void UGrabber::SetupInputComponent() {
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) {
		UE_LOG(LogTemp, Warning, TEXT("InputComponent found!"))

		///Bind input axis
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("InputComponent not found in %s's components!"), *(GetOwner()->GetName()))
	}
}

void UGrabber::Grab() {
	UE_LOG(LogTemp, Warning, TEXT("Grab key pressed!"))

	///LINE TRACE and see if we reach any actors with phjysics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();

	//If we hit something then attach a physics handle
	if(ActorHit)
		//Attach physics handle
		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None,										//Bone name
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true											//Allow Rotation
		);
}

void UGrabber::Release() {
	UE_LOG(LogTemp, Warning, TEXT("Grab key released!"))
	PhysicsHandle->ReleaseComponent();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	///Get the player view point this tick
	FVector playerLocation;
	FRotator playerRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT playerLocation, OUT playerRotation);

	FVector LineTraceEnd = playerLocation + playerRotation.Vector() * Reach;

	//If the physics handle is attached and it has something grabbed
	if (PhysicsHandle->GrabbedComponent)
		//Move the object that we are holding
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const {
	///Get the player view point this tick
	FVector playerLocation;
	FRotator playerRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT playerLocation, OUT playerRotation);

	FVector LineTraceEnd = playerLocation + playerRotation.Vector() * Reach;

	///Setup Query parameters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	///Line Trace (AKA Ray-cast) out to reach distance
	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		playerLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	///See what we hit
	AActor* ActorHit = Hit.GetActor();
	if (ActorHit) {
		UE_LOG(LogTemp, Warning, TEXT("Line trace hit: %s"), *(ActorHit->GetName()))
	}

	return Hit;
}

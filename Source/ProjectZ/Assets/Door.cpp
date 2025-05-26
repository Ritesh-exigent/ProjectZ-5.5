// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimationAsset.h"
#include "GameFramework/GameModeBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "ProjectZ/Characters/Player/SPlayer.h"


// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(Collider);

	SKMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SKeletalMeshComponent"));
	SKMesh->SetupAttachment(Collider);

	bReplicates = true;
	SetReplicateMovement(true);
	OpenAnimation = nullptr;
	bOpenDoor = false;
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADoor, bOpenDoor);
}

void ADoor::ToggleDoor()
{
	if (!SKMesh)
		return;

	bOpenDoor = !bOpenDoor;
	OnRep_ToggleDoor();
	//NetMulticast_SetDoorCollision(ECollisionEnabled::NoCollision);
}

void ADoor::OnRep_ToggleDoor()
{
	if (bOpenDoor)
	{
		if (OpenAnimation)
		{
			SKMesh->PlayAnimation(OpenAnimation, false);
		}
	}
	else if(!bOpenDoor)
	{
		if (CloseAnimation)
		{
			SKMesh->PlayAnimation(CloseAnimation, false);
		}
	}
}

void ADoor::NetMulticast_SetDoorCollision_Implementation(ECollisionEnabled::Type InType)
{
	Collider->SetCanEverAffectNavigation(false);
	Collider->SetCollisionEnabled(InType);
}

void ADoor::Interact(ASPlayer* InPlayer)
{
	if (HasAuthority())
	{
		ToggleDoor();
	}
}


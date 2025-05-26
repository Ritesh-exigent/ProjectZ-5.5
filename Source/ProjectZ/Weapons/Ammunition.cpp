// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammunition.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AAmmunition::AAmmunition()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(Collider);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	Mesh->SetupAttachment(Collider);

	Collider->OnComponentBeginOverlap.AddDynamic(this, &AAmmunition::OnBeginOverlap);

	bReplicates = true;
	FixedAmount = 5;
	MinRandomAmount = 6;
	MaxRandomAmount = 20;
}

// Called when the game starts or when spawned
void AAmmunition::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAmmunition::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{

	}
}

void AAmmunition::Server_InitDestroy_Implementation()
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor Destroy"));
		Destroy();
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("doesn't have the authority"));

}

void AAmmunition::InitDestroy()
{
	UE_LOG(LogTemp, Warning, TEXT("InitDestroy called!"));
	Server_InitDestroy();
}

int32 AAmmunition::GetDropAmmoAmount(bool bGetFixedAmount)
{
	return (bGetFixedAmount) ? FixedAmount : FMath::RandRange(MinRandomAmount, MaxRandomAmount);
}

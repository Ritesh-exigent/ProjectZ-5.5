// Fill out your copyright notice in the Description page of Project Settings.


#include "HDComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UHDComponent::UHDComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	
	C_Health = 100;
	MaxHealth = 200;

}


// Called when the game starts
void UHDComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


// Called every frame
void UHDComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHDComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHDComponent, C_Health);
}

void UHDComponent::TakeDamage(float InAmount)
{
	if (GetOwner()->HasAuthority())
	{
		Damage(InAmount);
	}
	else
		Server_TakeDamage(InAmount);
}

void UHDComponent::AddHealth(float InAmount)
{
	C_Health += InAmount;
	if (C_Health > MaxHealth)
		C_Health = MaxHealth;
	if (GetOwnerRole() == ROLE_AutonomousProxy)
		Server_UpdateHealth(C_Health);
}

void UHDComponent::Server_UpdateHealth_Implementation(float InHealth)
{
	C_Health = InHealth;
	OnRep_HealthUpdate();
}

void UHDComponent::Server_TakeDamage_Implementation(float InAmount)
{
	Damage(InAmount);
}

void UHDComponent::OnRep_HealthUpdate()
{
	//update health 
}

void UHDComponent::Damage(float InAmount)
{
	C_Health -= InAmount;
	if (C_Health <= 0)
	{
		C_Health = 0;
		OnDeath.ExecuteIfBound();
	}
}


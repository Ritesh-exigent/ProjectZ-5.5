// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"
#include "Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "./ProjectZ/Characters/Player/SPlayer.h"
// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	Weapons.SetNum(2);
	CurrentIndex = -1;
	// ...
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerOwner = Cast<ASPlayer>(GetOwner());
	if(PlayerOwner && GetOwnerRole() == ROLE_AutonomousProxy)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to cast the player owner locally"));
		Server_CastPlayerOwner();
	}
	UE_LOG(LogTemp, Warning, TEXT("Weapons slot: %d"), Weapons.Num());
}


// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWeaponComponent, PlayerOwner);
	DOREPLIFETIME(UWeaponComponent, CurrentWeapon);
	DOREPLIFETIME(UWeaponComponent, CurrentIndex);
	DOREPLIFETIME(UWeaponComponent, Weapons);
	DOREPLIFETIME(UWeaponComponent, CurrentWeaponType);
}

bool UWeaponComponent::AddWeapon(AWeapon* NewWeapon)
{
	if (!PlayerOwner)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerOwner is null in weapon component cpp"));
		return false;
	}
	if(Weapons.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Weapon slots, should be 2"));
		return false;
	}
	
	int Index = -1;
	if (!Weapons[0])
	{
		Index = 0;
		Weapons[0] = NewWeapon;
	}
	else if (!Weapons[1])
	{
		Index = 1;
		Weapons[1] = NewWeapon;
	}

	if (Index != -1)
	{
		NewWeapon->SetOwner(GetOwner());
		NewWeapon->SetPlayerOwner(PlayerOwner);
		FAttachmentTransformRules ATR = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
		ATR.bWeldSimulatedBodies = true;
		UE_LOG(LogTemp, Warning, TEXT("%s: Weapon Added"), *PlayerOwner->GetName());
		if(NewWeapon->AttachToComponent(PlayerOwner->GetMesh(), ATR, (Index == 0) ? WEAPON_SOCKET1 : WEAPON_SOCKET2))
		{
			NetMulticast_SetWeaponCollision(NewWeapon, ECollisionEnabled::NoCollision);
			UE_LOG(LogTemp, Warning, TEXT("NetMulticast called"));
		}
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon slots are full"));
	}
	return false;
}

void UWeaponComponent::EquipWeapon(int32 Index)
{
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		Server_EquipWeapon(Index);
	}
	else
	{
		check(Index < Weapons.Num());
		if (CurrentIndex == Index) return;

		if (Weapons[Index])
		{
			FAttachmentTransformRules ATR = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
			ATR.bWeldSimulatedBodies = true;
			if (CurrentWeapon)
			{
				CurrentWeapon->AttachToComponent(PlayerOwner->GetMesh(), ATR, (CurrentIndex == 0) ? WEAPON_SOCKET1 : WEAPON_SOCKET2);

				//holster the equipped one
			}
			//equip the selected one
			CurrentWeapon = Weapons[Index];
			CurrentWeapon->AttachToComponent(PlayerOwner->GetMesh(), ATR , CURRENT_WSOCKET);
			CurrentWeaponType = CurrentWeapon->GetWeaponProperties().WeaponType;
			OnRep_OnCurrentWeaponChanged();
			CurrentIndex = Index;
		}
	}

}

void UWeaponComponent::UseWeapon(bool bInValue)
{
	//role check must not be here inorder to work in single player mode
	if (GetOwnerRole() != ROLE_AutonomousProxy || !HasEquippedAnyWeapon())
		return;

	if (bInValue)
		CurrentWeapon->StartFiring();
	else
		CurrentWeapon->StopFiring();
}

bool UWeaponComponent::AddAmmo(int32 Amount)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (CurrentWeapon)
		{
			return CurrentWeapon->AddAmmo(Amount);
		}
	}
	Server_AddAmmo(Amount);
	return true;
}

bool UWeaponComponent::ReloadWeapon()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (!CurrentWeapon)
			return false;

		return CurrentWeapon->Reload();
	}
	Server_Reload();
	return true;
}

UAnimMontage* UWeaponComponent::GetCurrentWeaponMontage(EWeaponAnimType InType)
{
	if (!CurrentWeapon)
		return nullptr;
	return CurrentWeapon->GetAnimMontage(InType);
}

void UWeaponComponent::Server_CastPlayerOwner_Implementation()
{
	PlayerOwner = Cast<ASPlayer>(GetOwner());
	if(!PlayerOwner)
		UE_LOG(LogTemp, Warning, TEXT("Failed to cast the player owner on server"));
}

void UWeaponComponent::Server_AddWeapon_Implementation(int32 Index, AWeapon* Weapon)
{

	if (Index < 0 || Index > 1 || !Weapon)
		return;

	if (Weapons.Num() != 2)
		Weapons.SetNum(2);

	Weapons[Index] = Weapon;
	Weapons[Index]->SetOwner(GetOwner());
	Weapons[Index]->SetPlayerOwner(PlayerOwner);

	FAttachmentTransformRules ATR = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
	ATR.bWeldSimulatedBodies = true;
	if(Weapons[Index]->AttachToComponent(PlayerOwner->GetMesh(), ATR, (Index == 0) ? WEAPON_SOCKET1 : WEAPON_SOCKET2))
	{
		NetMulticast_SetWeaponCollision(Weapon, ECollisionEnabled::NoCollision);
	}
	

}

void UWeaponComponent::NetMulticast_SetWeaponCollision_Implementation(AWeapon* Weapon, ECollisionEnabled::Type InType)
{
	Weapon->bGenerateOverlapEventsDuringLevelStreaming = false;
	Weapon->SetCollision(InType);
}

void UWeaponComponent::Server_EquipWeapon_Implementation(int32 Index)
{
	EquipWeapon(Index);
}

void UWeaponComponent::Server_AddAmmo_Implementation(int32 Ammo)
{
	AddAmmo(Ammo);
}

void UWeaponComponent::Server_Reload_Implementation()
{
	ReloadWeapon();
}

void UWeaponComponent::OnRep_OnCurrentWeaponChanged()
{
	CurrentWeapon->UpdateAmmoHUD();
}


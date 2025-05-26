// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Bullet.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "./ProjectZ/UI/SPlayerHUD.h"
#include "Components/StaticMeshComponent.h"
#include "./ProjectZ/Characters/Player/SPlayer.h"
#include "DrawDebugHelpers.h"
// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent_Temporary"));
	SetRootComponent(Mesh);
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	Collider->SetupAttachment(Mesh);

	bReplicates = true;
	bFire = false;
	RateOfFire = 1.f;
	CurrentAmmo = 30;
	TotalAmmo = 30;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	switch (WeaponQuality) {
	case EWeaponQualityType::Grey: WeaponProperties.DamageMultiplier = 1.f;
		break;
	case EWeaponQualityType::Green: WeaponProperties.DamageMultiplier = 1.2f;
		break;
	case EWeaponQualityType::Blue: WeaponProperties.DamageMultiplier = 1.4f;
		break;
	case EWeaponQualityType::Purple: WeaponProperties.DamageMultiplier = 1.6f;
		break;
	case EWeaponQualityType::Orange: WeaponProperties.DamageMultiplier = 1.8f;
		break;
	default: WeaponProperties.DamageMultiplier = 1.f;
		break;
	}

	RateOfFire = 1 / WeaponProperties.RoundsPerSec;
	CurrentAmmo = WeaponProperties.MagSize;
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HandleDefaultFire(DeltaTime);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, PlayerOwner);
	DOREPLIFETIME_CONDITION(AWeapon, CurrentAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeapon, TotalAmmo, COND_OwnerOnly);
}

void AWeapon::HandleDefaultFire(float DeltaTime)
{
	if (bFire && PlayerOwner)
	{
		RateOfFire += DeltaTime;
		if (RateOfFire >= 1 / WeaponProperties.RoundsPerSec)
		{
			RateOfFire = 0.f;
			//play animation
			if (HasAuthority())
			{
				Fire();
			}
			else
			{
				Server_Fire();
			}
			//play sound
			if (!WeaponProperties.bAutomatic)
				StopFiring();
		}
	}
}

void AWeapon::Server_SpawnBullets_Implementation(const FTransform SpawnTransform)
{
	SpawnBullets(SpawnTransform);
}

void AWeapon::UpdateAmmoHUD()
{
	if (PlayerOwner)
	{
		PlayerOwner->SetCurrentAmmo(CurrentAmmo);
		PlayerOwner->SetTotalAmmo(TotalAmmo);
	}
}

void AWeapon::SetPlayerOwner(ASPlayer* InOwner)
{
	PlayerOwner = InOwner;
	UpdateAmmoHUD();
	/*if (PlayerOwner)
	{
		PlayerOwner->SetCurrentAmmo(CurrentAmmo);
		PlayerOwner->SetTotalAmmo(TotalAmmo);
	}*/
}

void AWeapon::SetCollision(ECollisionEnabled::Type InType)
{
	Collider->SetCollisionEnabled(InType);
}

void AWeapon::Interact(ASPlayer* InPlayer)
{
	if (InPlayer)
	{
		if (InPlayer->AddWeapon(this))
			UE_LOG(LogTemp, Warning, TEXT("WeaponAdded to the slot"));
	}
}

void AWeapon::StartFiring()
{
	bFire = true;
}

void AWeapon::Server_Fire_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("Server Fire Called"));
	Fire();
}

void AWeapon::Fire()
{
	if (CurrentAmmo <= 0)
	{
		CurrentAmmo = 0;
		//indiate to reload the weapon
		return;
	}
	

	if (PlayerOwner)
	{
		FHitResult FireHit;
		FVector StartLocation = PlayerOwner->GetCamera()->GetComponentLocation();
		FVector EndLocation = StartLocation + PlayerOwner->GetCamera()->GetForwardVector() * WeaponProperties.Range;
		FTransform SocketTransform = Mesh->GetSocketTransform(FName("S_Muzzle"));
		FCollisionQueryParams CQP;
		CQP.AddIgnoredActor(PlayerOwner);
		if (GetWorld()->LineTraceSingleByChannel(FireHit, StartLocation, EndLocation, ECC_GameTraceChannel1, CQP))
		{
			SocketTransform.SetRotation((FireHit.Location - SocketTransform.GetLocation()).Rotation().Quaternion());
			DrawDebugLine(GetWorld(), SocketTransform.GetLocation(), FireHit.Location, FColor::Red, false, 0.2f);
		}
		else
		{
			SocketTransform.SetRotation((EndLocation - SocketTransform.GetLocation()).Rotation().Quaternion());
			DrawDebugLine(GetWorld(), SocketTransform.GetLocation(), EndLocation, FColor::Blue, false, 0.2f);
		}
		//SpawnBullets(SocketTransform);
		SpawnBullets(SocketTransform);
		CurrentAmmo--;
		OnRep_OnAmmoStateChanged();
	}
}

void AWeapon::OnRep_OnAmmoStateChanged()
{
	UpdateAmmoHUD();
}

void AWeapon::StopFiring()
{
	RateOfFire = 1/WeaponProperties.RoundsPerSec;
	bFire = false;
}

void AWeapon::SpawnBullets(const FTransform& InTransform)
{
	if (ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(BulletClass, InTransform))
	{
		Bullet->SetDamage(WeaponProperties.BaseDamage * WeaponProperties.DamageMultiplier);
		Bullet->StartSimulation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn bullet"));
	}
}

bool AWeapon::AddAmmo(int32 Amount)
{
	if (TotalAmmo >= WeaponProperties.MaxAmmo)
	{
		TotalAmmo = WeaponProperties.MaxAmmo;
		return false;
	}

	TotalAmmo += Amount;
	UE_LOG(LogTemp, Warning, TEXT("Total Ammo: %d"), TotalAmmo);
	OnRep_OnAmmoStateChanged();
	return true;
}

bool AWeapon::Reload()
{
	if (IsReloaded())
		return false;

	if (TotalAmmo <= 0)
	{
		TotalAmmo = 0;
		return false;
	}
	else if (CurrentAmmo >= WeaponProperties.MagSize)
		return false;

	int32 NetAmmo = TotalAmmo - WeaponProperties.MagSize;
	if (NetAmmo < 0)
		NetAmmo = TotalAmmo;
	else
		NetAmmo = WeaponProperties.MagSize;
	CurrentAmmo = NetAmmo;
	TotalAmmo -= NetAmmo;
	OnRep_OnAmmoStateChanged();
	return true;;
}

UAnimMontage* AWeapon::GetAnimMontage(EWeaponAnimType InType)
{
	if (WeaponAnimations.Find(InType))
		return WeaponAnimations[InType];
	return nullptr;
}


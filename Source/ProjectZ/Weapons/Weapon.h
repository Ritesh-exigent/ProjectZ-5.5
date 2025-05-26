// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "./ProjectZ/Interfaces/WeaponInterface.h"
#include "./ProjectZ/Interfaces/InteractionInterface.h"
#include "Weapon.generated.h"

class ABullet;
class ASPlayer;
class UBoxComponent;
class UStaticMeshComponent;

#define MUZZLE_SOCKET "Mz_Socket"

UENUM()
enum EWeaponAnimType : uint8 
{
	W_Equipped,
	W_ADS,
	W_Walk,
	W_Run,
};

UENUM()
enum EWeaponType : uint8
{
	Unarmed = 0,
	Melle = 1,
	Pistol = 2,
	SMG = 3,
	AR = 4,
	LMG = 5,
	Launchers = 6,
};

UENUM()
enum class EWeaponQualityType : uint8
{
	Grey,	//Lowest Quality
	Green,
	Blue,
	Purple,
	Orange,	//Highest Quality
};

USTRUCT()
struct FWeaponProperties {

	GENERATED_BODY()

	FWeaponProperties(){
		WeaponType = EWeaponType::Melle;
		MagSize = 20;
		RecoilStrength = 1.f;
		ReloadTime = 3.f;
		RoundsPerSec = 5.f;
		BaseDamage = 10.f;
		bAutomatic = false;
		ReloadMontage = nullptr;
		DamageMultiplier = 1.f;
	}

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EWeaponType> WeaponType;
	UPROPERTY(EditAnywhere)
	int32 MagSize;
	UPROPERTY(EditAnywhere)
	int32 MaxAmmo;
	UPROPERTY(EditAnywhere)
	float RecoilStrength;
	UPROPERTY(EditAnywhere)
	float ReloadTime;
	UPROPERTY(EditAnywhere)
	float RoundsPerSec;
	UPROPERTY(EditAnywhere)
	float Range;
	UPROPERTY(EditAnywhere)
	float BaseDamage;
	UPROPERTY(EditAnywhere)
	bool bAutomatic;
	UPROPERTY(EditAnywhere)
	UAnimMontage* ReloadMontage;

	float DamageMultiplier;
};

UCLASS()
class PROJECTZ_API AWeapon : public AActor, public IInteractionInterface, public IWeaponInterface
{
	GENERATED_BODY()
	
public:	
	
	AWeapon();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Settings | Defaults")
	UBoxComponent* Collider;
	UPROPERTY(EditDefaultsOnly, Category = "Settings | Defaults")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, Category = "Settings | Properties")
	EWeaponQualityType WeaponQuality;
	UPROPERTY(EditDefaultsOnly, Category = "Settings | Properties")
	FWeaponProperties WeaponProperties;
	
	UPROPERTY(EditDefaultsOnly, Category = "Settings | Properties")
	TSubclassOf<ABullet> BulletClass;

	UPROPERTY(EditDefaultsOnly, Category = "Settings | Animation")
	TMap<TEnumAsByte<EWeaponAnimType>, UAnimMontage*>WeaponAnimations;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void HandleDefaultFire(float DeltaTime);

private:


	bool bFire;
	float RateOfFire;
	UPROPERTY(ReplicatedUsing = OnRep_OnAmmoStateChanged)
	int32 CurrentAmmo;
	UPROPERTY(Replicated)
	int32 TotalAmmo;

	UPROPERTY(Replicated)
	ASPlayer* PlayerOwner;

	void Server_SpawnBullets_Implementation(const FTransform SpawnTransform);

	UFUNCTION(Server, Reliable)
	void Server_Fire();
	void Server_Fire_Implementation();
	void Fire();

	UFUNCTION()
	void OnRep_OnAmmoStateChanged();

public:
	
	__inline UBoxComponent* GetCollider() { return Collider; }
	__inline UStaticMeshComponent* GetMesh() { return Mesh; }
	__inline FWeaponProperties& GetWeaponProperties() { return WeaponProperties; }
	__inline int32 GetCurrentAmmo() { return CurrentAmmo; }
	__inline int32 GetTotalAmmo() { return TotalAmmo; }
	__inline bool IsReloaded() { return CurrentAmmo == WeaponProperties.MagSize; }
	
	UFUNCTION(Server, Reliable)
	virtual void Server_SpawnBullets(const FTransform SpawnTransform);
	
	void UpdateAmmoHUD();
	void SetPlayerOwner(ASPlayer* InOwner);
	void SetCollision(ECollisionEnabled::Type InType);

	//InteracionInterface
	void Interact(ASPlayer* InPlayer) override;

	void StartFiring();
	void StopFiring();
	void SpawnBullets(const FTransform& InTransform) override;
	bool AddAmmo(int32 Amount);
	bool Reload();
	UAnimMontage* GetAnimMontage(EWeaponAnimType InType);

};

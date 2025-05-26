// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

#define CURRENT_WSOCKET "CurrWeapon_Socket"
#define WEAPON_SOCKET1 "Weapon1_Socket"
#define WEAPON_SOCKET2 "Weapon2_Socket"


class AWeapon;
class ASPlayer;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTZ_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UWeaponComponent();

protected:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:

	bool AddWeapon(AWeapon* NewWeapon);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetNextWeaponIndex() { return (CurrentIndex + 1) % 2; }
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(int32 Index);
	void UseWeapon(bool InValue);
	bool AddAmmo(int32 Amount);
	UFUNCTION(BlueprintCallable)
	bool ReloadWeapon();
	UAnimMontage* GetCurrentWeaponMontage(EWeaponAnimType InType);

	__inline AWeapon* GetCurrentWeapon() { return CurrentWeapon; }
	__inline int32 GetCurrentWeaponType() { return CurrentWeaponType; }
	__inline bool HasEquippedAnyWeapon() { return (CurrentWeapon) ? true : false; }


private:


	UPROPERTY(Replicated)
	ASPlayer* PlayerOwner;
	UPROPERTY(ReplicatedUsing = OnRep_OnCurrentWeaponChanged)
	AWeapon* CurrentWeapon;
	UPROPERTY(Replicated)
	int32 CurrentIndex;
	UPROPERTY(Replicated)
	TArray<AWeapon*> Weapons;
	UPROPERTY(Replicated)
	TEnumAsByte<EWeaponType> CurrentWeaponType;

	//UFUNCTION()
	//void OnRep_CurrentWeapon();

	UFUNCTION(Server, Reliable)
	void Server_CastPlayerOwner();
	void Server_CastPlayerOwner_Implementation();

	UFUNCTION(Server, Unreliable)
	void Server_AddWeapon(int32 Index, AWeapon* Weapon);
	void Server_AddWeapon_Implementation(int32 Index, AWeapon* Weapon);

	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticast_SetWeaponCollision(AWeapon* Weapon, ECollisionEnabled::Type InType);
	void NetMulticast_SetWeaponCollision_Implementation(AWeapon* Weapon, ECollisionEnabled::Type InType);

	UFUNCTION(Server, Unreliable)
	void Server_EquipWeapon(int32 Index);
	void Server_EquipWeapon_Implementation(int32 Index);
	
	UFUNCTION(Server, Unreliable)
	void Server_AddAmmo(int32 Ammo);
	void Server_AddAmmo_Implementation(int32 Ammo);

	UFUNCTION(Server, Unreliable)
	void Server_Reload();
	void Server_Reload_Implementation();

	UFUNCTION()
	void OnRep_OnCurrentWeaponChanged();


};

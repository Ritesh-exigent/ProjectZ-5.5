// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "./ProjectZ/Quest/QuestUtils.h"
#include "EnhancedInput/Public/InputAction.h"
#include "./ProjectZ/Characters/BaseCharacter.h"
#include "EnhancedInput/Public/InputActionValue.h"
#include "SPlayer.generated.h"

class AWeapon;
class USPlayerHUD;
class UInputAction;
class USPAnimInstance;
class UInputMappingContext;

#define MAX_WEAPON_SLOT 3

UCLASS()
class PROJECTZ_API ASPlayer : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASPlayer();

protected:
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category="Settings | Components")
	class USpringArmComponent* SpringArm;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category="Settings | Components")
	class UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Settings | Components")
	class UWeaponComponent* WComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Settings | Values")
	float InteractionRange;

private:

	UPROPERTY(EditDefaultsOnly, Category="Settings | Input")
	UInputMappingContext* IMC;
	UPROPERTY(EditDefaultsOnly, Category="Settings | Input")
	UInputAction* MoveAction;
	UPROPERTY(EditDefaultsOnly, Category="Settings | Input")
	UInputAction* LookAction;
	UPROPERTY(EditDefaultsOnly, Category="Settings | Input")
	UInputAction* SprintAction;
	UPROPERTY(EditDefaultsOnly, Category="Settings | Input")
	UInputAction* JumpAction;
	UPROPERTY(EditDefaultsOnly, Category="Settings | Input")
	UInputAction* FireAction;
	UPROPERTY(EditDefaultsOnly, Category="Settings | Input")
	UInputAction* ReloadAction;
	UPROPERTY(EditDefaultsOnly, Category="Settings | Input")
	UInputAction* InteractAction;
	UPROPERTY(EditDefaultsOnly, Category="Settings | Input")
	TSubclassOf<UUserWidget> PlayerHUDClass;
	/*UPROPERTY(EditDefaultsOnly, Category="Settings | Input")
	UInputAction* InteractAction;*/

	UPROPERTY(Replicated)
	float LookY;

	bool bSprint;
	FVector2D LookValue;
	USPlayerHUD* PlayerHUD;
	USPAnimInstance* AnimInst;
	FQuestDescription CurrentQuestDescription;

	void PerformMove(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Sprint(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Look(FVector2D LookVal);
	void Server_Look_Implementation(FVector2D LookVal);
	bool Server_Look_Validate(FVector2D LookVal);

	void Attack(const FInputActionValue& Value);
	void StopAttack(const FInputActionValue& Value);
	void ReloadWeapon(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void Server_BeginInteract();
	void Server_BeginInteract_Implementation();
	
	UFUNCTION(Server, Reliable)
	void Server_BeginReload();
	void Server_BeginReload_Implementation();
	
	UFUNCTION(Server, Reliable)
	void NetMulticast_PlayReloadMontage(UAnimMontage* InMontage);
	void NetMulticast_PlayReloadMontage_Implementation(UAnimMontage* InMontage);

	UFUNCTION(Server, Reliable)
	void Server_MoveUpdate(float DeltaTime);
	void Server_MoveUpdate_Implementation(float DeltaTime);

	void MoveUpdate(float DeltaTime);

	void BeginInteract();
	void InitPlayerHUD();

public:

	__inline UCameraComponent* GetCamera() { return Camera; }
	__inline UWeaponComponent* GetWeaponComponent() { return WComp; }
	__inline USPlayerHUD* GetPlayerHUD() { return PlayerHUD; }
	__inline USPAnimInstance* GetAnimInstance() { return AnimInst; }
	__inline float GetLookYValue() { return LookY; }
	
	void SetCurrentAmmo(int32 InCurrentAmmo);
	void SetTotalAmmo(int32 InTotalAmmo);

	bool AddWeapon(AWeapon* NewWeapon);
	void UpdateAnimInstanceMontage(UAnimMontage* InMontage);
};

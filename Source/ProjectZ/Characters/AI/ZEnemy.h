// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectZ/Characters/BaseCharacter.h"
#include "ZEnemy.generated.h"

class AAmmunition;
class UBehaviorTree;
class AZAIController;
class AZEnemyManager;
class UZAnimInstance;
class UPerceptionComponent;

UCLASS()
class PROJECTZ_API AZEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	
	AZEnemy();

protected:
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category="Settings | Components")
	UPerceptionComponent* PerceptionComp;
	UPROPERTY(EditDefaultsOnly, Category = "Settings | Values")
	TArray<TSubclassOf<AAmmunition>> Drops;
	UPROPERTY(EditDefaultsOnly, Category = "Settings | Values")
	TArray<UAnimMontage*> AttackMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Settings | Values")
	TSubclassOf<AZAIController> ControllerClass;

public:
		
	UFUNCTION(BlueprintCallable)
	AZEnemyManager* GetManager() { return Manager; };

	__inline void SetPoolID(int32 InID) { PoolID = InID; }
	__inline void SetManager(AZEnemyManager* InManager) { Manager = InManager; }

	void Attack(bool bOverride);

	__inline UBehaviorTree* GetBehaviorTree() { return BTAsset; }
	UFUNCTION(BlueprintCallable)
	UPerceptionComponent* GetPerceptionComponent() { return PerceptionComp; }
	UFUNCTION(BlueprintImplementableEvent)
	void StartDestroy();

	UFUNCTION()
	void OnDeath();

	//..........................
	void Reset();
	void Init();

private:

	UPROPERTY(EditDefaultsOnly, Category = "Settings | BTree")
	UBehaviorTree* BTAsset;

	UZAnimInstance* AnimInst;

	int32 PoolID;
	AZAIController* ZController;
	AZEnemyManager* Manager;
	friend class AZEnemyManager;

	void DropAmmunitions();
	void InitDestroy();
	void PlayAttackMontage(bool bOverride);
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* InMontage, bool bInterrupted);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_PlayAttackMontage(bool bOverride);
	void NetMulticast_PlayAttackMontage_Implementation(bool bOverride);
};

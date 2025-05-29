// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "./ProjectZ/Characters/AI/ZEnemy.h"
#include "ProjectZ/Characters/AI/Controllers/ZAIController.h"
#include "ZEnemyManager.generated.h"

class AZGameState;
class UZSpawnManager;

UCLASS()
class PROJECTZ_API AZEnemyManager : public AActor
{
	GENERATED_BODY()
	
public:	

	AZEnemyManager();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Settings | Values")
	UZSpawnManager* ZSpawnComp;
	UPROPERTY(EditAnywhere, Category="Settings | Enemies")
	TSubclassOf<AZAIController> ZAIController;
	UPROPERTY(EditAnywhere, Category="Settings | Enemies")
	TArray<TSubclassOf<AZEnemy>> NormalEnemies;
	UPROPERTY(EditAnywhere, Category="Settings | Enemies")
	TSubclassOf<AZEnemy> RangeEnemy;
	UPROPERTY(EditAnywhere, Category="Settings | Enemies")
	TSubclassOf<AZEnemy> BossEnemy;
	
	UPROPERTY(EditAnywhere, Category="Settings | Values")
	int32 MaxEnemies;
	UPROPERTY(EditAnywhere, Category="Settings | Values")
	int32 MaxSpawnedEnemies;
	UPROPERTY(EditAnywhere, Category="Settings | Values")
	int32 SpawnOtherOnRemaining;
	UPROPERTY(EditAnywhere, Category="Settings | Values")
	float SpawnRadius;
	UPROPERTY(EditAnywhere, Category = "Settings | Values", meta = (ClampMin = "0", ClampMax = "2"))
	float EnemySpawnMultiplier;
	UPROPERTY(EditAnywhere, Category="Settings | Values")
	int32 WaveCooldownTimer;

	int32 Wave;
	int32 WaveNumEnemies;
	int32 DeathCount;
	int32 EnemyCount;

	bool bCanSpawnEnemies;
	float CurrentTimer;

	AZGameState* ZGameState;

	void AsyncSpawnEnemies();
	void SpawnEnemies();
	void PrepareNextWave();
	void FinishWave();


	friend class AZEnemy;

public:
	UFUNCTION(BlueprintCallable)
	void OnDeath(AActor* Initiator);

	UFUNCTION(BlueprintCallable)
	void InitEnemies();

};

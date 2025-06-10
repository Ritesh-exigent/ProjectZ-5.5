// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
#include "./ProjectZ/Characters/AI/ZEnemy.h"
#include "ProjectZ/Characters/AI/Controllers/ZAIController.h"
#include "ZEnemyManager.generated.h"

class AZSpawn;
class AZGameState;
class UZSpawnManager;

USTRUCT()
struct FEnemyPoolInfo {
	GENERATED_BODY()
public:


	FEnemyPoolInfo() {
		PoolID = 0;
		PoolSize = 100;
	}

	UPROPERTY(EditAnywhere)
	uint32 PoolID;
	UPROPERTY(EditAnywhere)
	int32 PoolSize;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AZEnemy> EnemyClass;
};

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
	TArray<FEnemyPoolInfo> EnemyPoolInfo;

	TMap<int32, TUniquePtr<TQueue<AZEnemy*>>>Pools;
	
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

	bool bCanSpawnEnemies;
	float CurrentTimer;

	UPROPERTY(EditAnywhere, Category="Settings | Values")
	TArray<TSoftObjectPtr<AZSpawn>> Spawners;
	AZGameState* ZGameState;

	void AsyncSpawnEnemies();
	void SpawnEnemies();
	void PrepareNextWave();
	void FinishWave();
	//......................................................
	void DispatchSpawn();

	void InitPools();

	friend class AZEnemy;

public:
	int32 EnemyCount;

	UFUNCTION(BlueprintCallable)
	void OnDeath(AActor* Initiator);

	UFUNCTION(BlueprintCallable)
	void InitEnemies();

	UFUNCTION(BlueprintCallable)
	void SetSpawners(TArray<AZSpawn*> InSpawners);
	
	//........................................
	UFUNCTION(BlueprintCallable)
	AZEnemy* SpawnFromPool(int32 ID, FVector Location, FRotator Rotation = FRotator::ZeroRotator);
};

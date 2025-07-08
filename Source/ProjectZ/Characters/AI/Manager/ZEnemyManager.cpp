// Fill out your copyright notice in the Description page of Project Settings.


#include "ZEnemyManager.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "./ProjectZ/GameModes/ZGameMode.h"
#include "../Spawn/ZSpawnManager.h" //tbd
#include "./ProjectZ/GameStates/ZGameState.h"
#include "../Spawn/ZSpawn.h"

DECLARE_LOG_CATEGORY_EXTERN(LogZEManager, Warning, All);
DEFINE_LOG_CATEGORY(LogZEManager);

// Sets default values
AZEnemyManager::AZEnemyManager()
{
	PrimaryActorTick.bCanEverTick = false;
	
	ZSpawnComp = CreateDefaultSubobject<UZSpawnManager>("ZSpawnComponent");

	MaxEnemies = 10;
	MaxSpawnedEnemies = 5;
	SpawnOtherOnRemaining = 1;
	SpawnRadius = 1000.f;
	EnemySpawnMultiplier = 1.f;
	WaveCooldownTimer = 10.f;
	
	Wave = 1;
	WaveNumEnemies = MaxSpawnedEnemies;
	DeathCount = 0;
	EnemyCount = 0;
}

void AZEnemyManager::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundSpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), SpawnClass, FoundSpawners);
	UE_LOG(LogTemp, Warning, TEXT("Spawners Num: %d"), FoundSpawners.Num());
	for (AActor* Actor : FoundSpawners)
	{
		AZSpawn* Spawn = Cast<AZSpawn>(Actor);
		if (Spawn)
		{
			Spawn->SetManager(this);
			Spawns.Add(Spawn);
		}
	}

	if (SpawnOtherOnRemaining > MaxSpawnedEnemies)
		SpawnOtherOnRemaining = MaxSpawnedEnemies;

	ZGameState = GetWorld()->GetAuthGameMode()->GetGameState<AZGameState>();
	PrepareNextWave();
	InitPools();
}

void AZEnemyManager::PrepareNextWave()
{
	WaveNumEnemies = EnemySpawnMultiplier * MaxSpawnedEnemies * Wave;
	WaveNumEnemies = FMath::Min(WaveNumEnemies, MaxEnemies);

	/*FTimerHandle CooldownTimer;
	GetWorld()->GetTimerManager().SetTimer(CooldownTimer, [this]() {
		DispatchSpawn();
	}, 0.1f, false, WaveCooldownTimer);*/
	UE_LOG(LogTemp, Warning, TEXT("WaveNum: %d, EnemyNum: %d"), Wave, WaveNumEnemies);
}

void AZEnemyManager::FinishWave()
{
	Wave++;

	if (ZGameState)
	{
		ZGameState->UpdateEnemyWaveInfo(FEnemyWaveInfo(Wave, WaveCooldownTimer));
	}

	EnemyCount = 0;
	PrepareNextWave();
}

void AZEnemyManager::DispatchSpawn()
{
	if (Spawns.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("no spawners found to spawn enemies!"));
		return;
	}

	int32 TotalNum = MaxSpawnedEnemies;
	if (WaveNumEnemies < MaxSpawnedEnemies)
	{
		TotalNum = WaveNumEnemies;
		WaveNumEnemies = 0;
	}
	else
		WaveNumEnemies -= TotalNum;

	int32 HalfNum = TotalNum / Spawns.Num();
	if (HalfNum <= 0)
		HalfNum = TotalNum;
	UE_LOG(LogTemp, Warning, TEXT("TotalNum: %d, HalfNum: %d"), TotalNum, HalfNum);
	for (int32 i=0; i<Spawns.Num(); ++i)
	{
		if (TotalNum - HalfNum < 0)
			HalfNum = TotalNum;

		int32 RandomSpawnIndex = FMath::RandRange(0, Spawns.Num() - 1);
		if (Spawns[RandomSpawnIndex]->IsSpawning())
			Spawns[RandomSpawnIndex]->AddSpawnCount(HalfNum);
		else
			Spawns[RandomSpawnIndex]->BeginSpawn(HalfNum);

		TotalNum -= HalfNum;
		if (TotalNum <= 0)
			break;
	}
	
}

void AZEnemyManager::InitPools()
{
	if (EnemyPoolInfo.IsEmpty())
	{
		GLog->Log(ELogVerbosity::Error, TEXT("EnemyPoolInfo is empty in enemy manager BP"));
		return;
	}
	FActorSpawnParameters ASP;
	ASP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (FEnemyPoolInfo PoolInfo : EnemyPoolInfo)
	{
		if (!PoolInfo.EnemyClass)
		{
			GLog->Logf(ELogVerbosity::Fatal, TEXT("Pool Enemy class is nullptr for id: %d"), PoolInfo.PoolID);
			continue;
		}

		if (!Pools.Contains(PoolInfo.PoolID))
		{
			Pools.Add(PoolInfo.PoolID, MakeUnique<TQueue<AZEnemy*>>());
		}

		for (int32 i = 0; i < PoolInfo.PoolSize;)
		{
			AZEnemy* Enemy = GetWorld()->SpawnActor<AZEnemy>(PoolInfo.EnemyClass, FVector(0.f, 0.f, -1000.f), FRotator::ZeroRotator, ASP);
			if (Enemy)
			{

				if(Pools[PoolInfo.PoolID]->Enqueue(Enemy))
				{
					FTimerHandle EnemyHandle;
					GetWorld()->GetTimerManager().SetTimer(EnemyHandle, [Enemy]() {
						Enemy->Reset();
						}, 0.1f, false, 2.f);
					Enemy->SetPoolID(PoolInfo.PoolID);
					++i;
				}
			}
		}
	}
}

AZEnemy* AZEnemyManager::SpawnFromPool(int32 ID, FVector Location, FRotator Rotation)
{
	if (Pools.Contains(ID) && !Pools[ID]->IsEmpty())
	{
		AZEnemy* SpawnedEnemy = nullptr;
		if (Pools[ID]->Dequeue(SpawnedEnemy))
		{
			SpawnedEnemy->Init();
			SpawnedEnemy->SetActorLocation(Location);
			SpawnedEnemy->SetActorRotation(Rotation);
			SpawnedEnemy->SetManager(this);
			EnemyCount++;
			return SpawnedEnemy;
		}
	}
	return nullptr;
}

void AZEnemyManager::AddToPool(int32 ID, AZEnemy* InEnemy)
{
	if (Pools.Contains(ID))
	{
		Pools[ID]->Enqueue(InEnemy);
	}
}

void AZEnemyManager::OnDeath(AActor* Initiator)
{
	--EnemyCount;
	UE_LOG(LogTemp, Warning, TEXT("EnemyCount: %d"), EnemyCount);
	if (EnemyCount <= SpawnOtherOnRemaining && WaveNumEnemies > 0 )
	{
		DispatchSpawn();
	}
	if (EnemyCount <= 0)
	{
		EnemyCount = 0;
		FinishWave();
	}
}
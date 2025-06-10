// Fill out your copyright notice in the Description page of Project Settings.


#include "ZEnemyManager.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "./ProjectZ/GameModes/ZGameMode.h"
#include "../Spawn/ZSpawnManager.h" //tbd
#include "./ProjectZ/GameStates/ZGameState.h"
#include "../Spawn/ZSpawn.h"

// Sets default values
AZEnemyManager::AZEnemyManager()
{
	PrimaryActorTick.bCanEverTick = true;
	
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

	bCanSpawnEnemies = false;
	CurrentTimer = 0.f;
}

void AZEnemyManager::BeginPlay()
{
	Super::BeginPlay();

	if (SpawnOtherOnRemaining > MaxSpawnedEnemies)
		SpawnOtherOnRemaining = MaxSpawnedEnemies;

	ZGameState = GetWorld()->GetAuthGameMode()->GetGameState<AZGameState>();

	//................
	InitPools();
}

void AZEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bCanSpawnEnemies)
	{
		CurrentTimer += DeltaTime;
		if (CurrentTimer >= WaveCooldownTimer)
		{	
			AsyncSpawnEnemies();
			//SpawnEnemies();
			CurrentTimer = 0;
			bCanSpawnEnemies = false;
		}
	}
}

void AZEnemyManager::AsyncSpawnEnemies()
{
	AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [this]() {SpawnEnemies(); });
}

void AZEnemyManager::SpawnEnemies()
{
	if(WaveNumEnemies <= 0) return;

	int32 EnemiesToSpawn = WaveNumEnemies - MaxSpawnedEnemies;
	EnemiesToSpawn = (EnemiesToSpawn < 0) ? WaveNumEnemies : MaxSpawnedEnemies;
	WaveNumEnemies -= EnemiesToSpawn;
	if (WaveNumEnemies < 0)
		WaveNumEnemies = 0;
		

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if(NavSys)
	{
		
		//for (int32 i = 0; i < EnemiesToSpawn; ++i)
		//{	
			AsyncTask(ENamedThreads::GameThread, [NavSys, EnemiesToSpawn, this]() {
				
				ZSpawnComp->CurateZombieSpawn(EnemiesToSpawn, ESpawnType::Spread);
				/*int32 i = 0;
				int32 NumPlayer = GetWorld()->GetAuthGameMode()->GetNumPlayers();
				APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), FMath::RandRange(0, NumPlayer - 1));
				while (i < EnemiesToSpawn)
				{
					FNavLocation RandomLocation;
					if (NavSys->GetRandomReachablePointInRadius(PlayerPawn->GetActorLocation(), SpawnRadius, RandomLocation))
					{
						FActorSpawnParameters ASP;
						ASP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
						AZAIController* ZController = GetWorld()->SpawnActor<AZAIController>(ZAIController.Get(), FVector::ZeroVector, FRotator::ZeroRotator, ASP);
						if (ZController)
						{
							RandomLocation.Location.Z += 96.f;
							AZEnemy* ZEnemy = GetWorld()->SpawnActor<AZEnemy>(NormalEnemies[0].Get(), RandomLocation.Location, FRotator::ZeroRotator, ASP);
							if(ZEnemy)
							{
								ZEnemy->Manager = this;
								ZController->Possess(ZEnemy);
								++EnemyCount;
								++i;
							}
						}
					}

				}*/
			});
		//}
	}
}

void AZEnemyManager::PrepareNextWave()
{
	WaveNumEnemies = EnemySpawnMultiplier * MaxSpawnedEnemies * Wave;
	WaveNumEnemies = FMath::Min(WaveNumEnemies, MaxEnemies);
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
	CurrentTimer = 0.f;
	bCanSpawnEnemies = true;
	PrepareNextWave();
}

void AZEnemyManager::DispatchSpawn()
{
	if (Spawners.Num() <= 0)
		return;

	int32 ZombiesPerSpawn = MaxEnemies / Spawners.Num();
	for (TSoftObjectPtr<AZSpawn> SpawnPoint : Spawners)
	{
		if (SpawnPoint)
		{
			SpawnPoint->SetManager(this);
			SpawnPoint->BeginSpawn(ZombiesPerSpawn);
		}
		else
			GLog->Log("SpawnPoint is null");
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
				//Enemy->SetActorHiddenInGame(true);
				Pools[PoolInfo.PoolID]->Enqueue(Enemy);
				++i;
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
			//activate enemy
			return SpawnedEnemy;
		}
	}
	return nullptr;
}

void AZEnemyManager::OnDeath(AActor* Initiator)
{
	UE_LOG(LogTemp, Warning, TEXT("onDeath Called by: %s"), *Initiator->GetName());
	--EnemyCount;
	if (EnemyCount <= SpawnOtherOnRemaining && WaveNumEnemies > 0 )
	{
		SpawnEnemies();
		//AsyncTask(ENamedThreads::GameThread, [this]() {SpawnEnemies(); });
	}
	if (EnemyCount <= 0)
	{
		EnemyCount = 0;
		FinishWave();
	}
}

void AZEnemyManager::InitEnemies()
{
	DispatchSpawn();
	/*bCanSpawnEnemies = true;
	PrepareNextWave();*/
}

void AZEnemyManager::SetSpawners(TArray<AZSpawn*> InSpawners)
{
	Spawners.Empty();
	//Spawners = InSpawners;
}


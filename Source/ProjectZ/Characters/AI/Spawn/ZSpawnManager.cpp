// Fill out your copyright notice in the Description page of Project Settings.


#include "ZSpawnManager.h"
#include "Kismet/GameplayStatics.h"
#include "ZSpawn.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"
#include "../Manager/ZEnemyManager.h"

UZSpawnManager::UZSpawnManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	bShouldSpawnZombies = true;
	bShouldForceSpawn = false;
}

void UZSpawnManager::BeginPlay()
{
	Super::BeginPlay();
	CurrentSpawnType = ESpawnType::Spread;

	GetWorld()->GetTimerManager().SetTimer(SphereHandle, this, &UZSpawnManager::DistanceSphereTrace, .1f, true);
}

void UZSpawnManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UZSpawnManager::DistanceSphereTrace()
{

	if (CurrentRadiusOfSphere >= MaxRadiusOfSphere) {
		CurrentRadiusOfSphere = 0;
		FinalSpawners = ClosestSpawners.Array();
		ClosestSpawners.Empty();
	}

	FVector Origin;
	TArray<FOverlapResult> HitResult;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator) {
		APlayerController* PC = Iterator->Get();
		if (PC) {
			APawn* Player = PC->GetPawn();
			if (Player) {
				Origin = Player->GetActorLocation();
				FCollisionQueryParams Params;
				Params.AddIgnoredActor(Player);
				
				bool bHit = GetWorld()->OverlapMultiByChannel(
					HitResult,
					Origin,
					FQuat::Identity,
					ECC_GameTraceChannel7,
					FCollisionShape::MakeSphere(CurrentRadiusOfSphere),
					Params
				);
				//DrawDebugSphere(GetWorld(), Origin, CurrentRadiusOfSphere, 12, FColor::Blue, false, 2.f);

				if (bHit) {
					//UE_LOG(LogTemp, Warning, TEXT("Hit for spawner"));
					for (const FOverlapResult& Hit : HitResult)
					{
						if (AActor* Actor = Hit.GetActor()) {

							if (Hit.GetActor()->IsA<AZSpawn>()) {
								//UE_LOG(LogTemp, Warning, TEXT("Spawner name: %s"), *Hit.GetActor()->GetName());
								AZSpawn* Spawner = Cast<AZSpawn>(Actor);
								Spawner->SetManager(Cast<AZEnemyManager>(GetOwner()));
								ClosestSpawners.Add(Spawner);
							}
						}
					}
				}
				/*else
					UE_LOG(LogTemp, Warning, TEXT("Hit Failed for spawner"));*/

				CurrentRadiusOfSphere += 1000;
			}
		}
	}
}

void UZSpawnManager::CurateZombieSpawn(int num, ESpawnType NewSpawnType)
{
	int32 Param = 1;
	switch (NewSpawnType) {
	case ESpawnType::Horde:
		Param = 3;
		break;
	case ESpawnType::Spread:
		Param = 2;
		break;
	case ESpawnType::Pop:
		Param = 1;
		break;
	}

	VarienceOfSpawning = ScaleByNumber(num, Param);
	UE_LOG(LogTemp, Warning, TEXT("variance %d"), VarienceOfSpawning);

	if (VarienceOfSpawning == 0 || num == 0 ) return;
	if (num < VarienceOfSpawning) VarienceOfSpawning = num;
	NumberOfZombies = num;
	int VarienceCheck=0;

	TArray<AZSpawn*> ValidSpawners;
	ValidSpawners.Reserve(FinalSpawners.Num());
	if (!bShouldForceSpawn) {
		for (AZSpawn* Spawner : FinalSpawners) {

			UE_LOG(LogTemp, Error, TEXT("read"));
			if (Spawner->IsValidSpawner) {
				ValidSpawners.Add(Spawner);
				VarienceCheck++;
			}
			if (VarienceCheck == VarienceOfSpawning) break;
		}
	}
	else {
		for (AZSpawn* Spawner : FinalSpawners) {
			ValidSpawners.Add(Spawner);
			VarienceCheck++;
			if (VarienceCheck == VarienceOfSpawning) break;
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("Final Spawn : %d"), FinalSpawners.Num());
	//UE_LOG(LogTemp, Warning, TEXT("Valid Spawn : %d"), ValidSpawners.Num());
	if (ValidSpawners.IsEmpty()) return;

	AmountToSpawn = num / VarienceOfSpawning;
	VarienceOfSpawning = FMath::Min(VarienceOfSpawning, ValidSpawners.Num());

	ZombiesSpawned = 0;
	count = 0;
	UE_LOG(LogTemp, Warning, TEXT("Time Spawn"));
	
	GetWorld()->GetTimerManager().SetTimer(SpawnHandle, [this,ValidSpawners,num,NewSpawnType]()
	{
		if (NumberOfZombies <= 0) {
			GetWorld()->GetTimerManager().ClearTimer(SpawnHandle);
			
			return;
		}
		else if (count >= VarienceOfSpawning) {
			count = 0;
		}
		else if (count >= ValidSpawners.Num()){
			count = 0;
		}
		else {
			if (!bShouldSpawnZombies) {
				bShouldSpawnZombies = true;
				return;
			}
			TimedSpawn(ValidSpawners);
			if (!DidItWork) {
				GetWorld()->GetTimerManager().ClearTimer(SpawnHandle);
				CurateZombieSpawn(num - ZombiesSpawned, NewSpawnType);
			}
		}

	}, .2f, true);
}

void UZSpawnManager::TimedSpawn(TArray<AZSpawn*> ValidSpawners)
{
	if (ValidSpawners[count]) {
		AZSpawn* Spawner = ValidSpawners[count];
		if (Spawner->IsValidSpawner) {
			UE_LOG(LogTemp, Error, TEXT("SpawnerRan"));
			Spawner->AllSpawn(AmountToSpawn);
			ZombiesSpawned += AmountToSpawn;
			NumberOfZombies = NumberOfZombies - AmountToSpawn;
			//UE_LOG(LogTemp, Error, TEXT("read %d"),AmountToSpawn);
			if (NumberOfZombies < AmountToSpawn) {
				AmountToSpawn = NumberOfZombies;
			}
			count++;
			
		}
		else
		{
			DidItWork = false;
		}
	}
}

int UZSpawnManager::ScaleByNumber(int num,int OutputRangeCoeff)
{
	int ScaleCoeff = 0;
	switch (OutputRangeCoeff) {
	case 1:
		 ScaleCoeff = 99999;
		 break;
	case 2:
		ScaleCoeff = 20;
		break;
	case 3:
		ScaleCoeff = 40;
		break;
	}

	return ((OutputRangeCoeff - 1) * num) / ScaleCoeff + (OutputRangeCoeff - (1 * (num / (ScaleCoeff * 2))));
}

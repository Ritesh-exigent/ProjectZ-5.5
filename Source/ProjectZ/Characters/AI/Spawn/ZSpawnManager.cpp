// Fill out your copyright notice in the Description page of Project Settings.


#include "ZSpawnManager.h"
#include "Kismet/GameplayStatics.h"
#include "ZSpawn.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "Engine/OverlapResult.h"

UZSpawnManager::UZSpawnManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	bShouldSpawnZombies = true;
	bShouldForceSpawn = false;
}

void UZSpawnManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("exists"));
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
					ECC_GameTraceChannel1,
					FCollisionShape::MakeSphere(CurrentRadiusOfSphere),
					Params
				);

				if (bHit) {
					for (const FOverlapResult& Hit : HitResult)
					{
						if (AActor* Actor = Hit.GetActor()) {

							if (Hit.GetActor()->IsA<AZSpawn>()) {
								AZSpawn* Spawner = Cast<AZSpawn>(Actor);
								ClosestSpawners.Add(Spawner);
							}
						}
					}
				}
				DrawDebugSphere(GetWorld(), Origin, CurrentRadiusOfSphere, 32, FColor::Red, false, 1.0f);

				CurrentRadiusOfSphere += 1000;
			}
		}
	}
}

void UZSpawnManager::CurateZombieSpawn(int num, ESpawnType NewSpawnType)
{
	switch (NewSpawnType) {
	case ESpawnType::Horde:
		VarienceOfSpawning = ScaleByNumber(num, 3);

		UE_LOG(LogTemp, Warning, TEXT("%d"), VarienceOfSpawning);
		break;
	case ESpawnType::Spread:
		VarienceOfSpawning = ScaleByNumber(num, 2);
	UE_LOG(LogTemp, Warning, TEXT("%d"),VarienceOfSpawning);
		break;
	case ESpawnType::Pop:
		VarienceOfSpawning = ScaleByNumber(num, 1);
		UE_LOG(LogTemp, Warning, TEXT("%d"), VarienceOfSpawning);
		break;
	}

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

	if (ValidSpawners.IsEmpty()) return;

	AmountToSpawn = num / VarienceOfSpawning;
	VarienceOfSpawning = FMath::Min(VarienceOfSpawning, ValidSpawners.Num());

	count = 0;
	
	UE_LOG(LogTemp, Warning, TEXT("readbefore"));
		GetWorld()->GetTimerManager().SetTimer(SpawnHandle, [this,ValidSpawners]()
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
					
				}

		}, .2f, true);
}

void UZSpawnManager::TimedSpawn(TArray<AZSpawn*> ValidSpawners)
{
	
		if (ValidSpawners[count]) {
			AZSpawn* Spawner = ValidSpawners[count]; 
			Spawner->AllSpawn(AmountToSpawn);
			NumberOfZombies = NumberOfZombies - AmountToSpawn;
			//UE_LOG(LogTemp, Error, TEXT("read %d"),AmountToSpawn);
			if (NumberOfZombies < AmountToSpawn) {
				AmountToSpawn = NumberOfZombies;
			}
			count++;
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

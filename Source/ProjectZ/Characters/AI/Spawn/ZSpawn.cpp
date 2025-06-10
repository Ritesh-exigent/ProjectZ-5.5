// Fill out your copyright notice in the Description page of Project Settings.


#include "ZSpawn.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Async/Async.h"
#include "ProjectZ/Characters/Player/SPlayer.h"
#include "../ZEnemy.h"
#include "../Manager/ZEnemyManager.h"



AZSpawn::AZSpawn()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;
	SphereComponent->InitSphereRadius(MaxSpawningRadius);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));
	
}


void AZSpawn::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AZSpawn::OnSphereOverlapBegin);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AZSpawn::OnSphereOverlapEnd);
		
}

void AZSpawn::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<ASPlayer>())
	{
		CurrentlyOverlappingActors.Add(OtherActor);
		IsValidSpawner = false;
	}

	
}

void AZSpawn::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CurrentlyOverlappingActors.Remove(OtherActor);
	IsValidSpawner = false;
}



bool AZSpawn::CheckLos(FVector Start, FVector End)
{
	FHitResult HitResult;
	FCollisionQueryParams CParams;
	CParams.AddIgnoredActor(this);

	bool Hit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CParams);
	DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Blue, false, .1f);
	
	if (HitResult.GetActor() && HitResult.GetActor()->IsA<ASPlayer>())
	{
	return true;

	}
	return false;
}



float AZSpawn::DistanceCalc(FVector Start, FVector End)
{
	float X = FMath::Square(Start.X - End.X);
	float Y = FMath::Square(Start.Y - End.Y);
	float Z = FMath::Square(Start.Z - End.Z);
	
	
	return (FMath::Sqrt(X+Y+Z));
}


//void AZSpawn::SpawnInBatch(int num)
//{
//	//int ToSpawn = FMath::Min(SpawnBatchSize, SpawnTotal - SpawnedCount);
//	for (int i = 0; i < num; i++) {
//		if (SpawnBatchSize > num) {
//			SpawnBatchSize = num;
//		}
//		SpawnZombie(SpawnBatchSize);
//		SpawnedCount=SpawnedCount+SpawnBatchSize;
//		if (SpawnedCount >= num) break;
//	}
//	
//}

void AZSpawn::SpawnZombie(int NumToSpawn)
{
	
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSys)
	{
		//todo- async loop
			for (int i = 0; i < NumToSpawn; i++) {

				FNavLocation RandomLoc;
				if (NavSys->GetRandomPointInNavigableRadius(GetActorLocation(), MaxSpawningRadius, RandomLoc))
				{
					FActorSpawnParameters Params;
					Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

					AsyncTask(ENamedThreads::GameThread, [this, RandomLoc, Params]() {
						AZEnemy* SpawnedActor = GetWorld()->SpawnActor<AZEnemy>(CubeClass, RandomLoc.Location + FVector(0.f, 0.f, 96.f), GetActorRotation(), Params);
						if (SpawnedActor)
						{
							SpawnedActor->SetManager(Manager);
							Manager->EnemyCount += 1;
						}
						});
				}	
			}
	}

}

//void AZSpawn::SpawnZombieAsync(FVector RandomVector, TFunction<void(AActor*)> Callback)
//{
//	TSharedRef<TPromise<AActor*>> PromiseOfZombie=MakeShared<TPromise<AActor*>>();
//	TFuture<AActor*> FutureOfZombie = PromiseOfZombie->GetFuture();
//
//	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, PromiseOfZombie,RandomVector]()
//		{
//
//			FVector SpawnLocation = RandomVector + GetActorLocation();
//			FRotator SpawnRotation = GetActorRotation();
//
//			
//				{
//
//					AActor* SpawnedActor = nullptr;
//
//					if (GetWorld() && CubeClass)  
//					{

//					}
//					PromiseOfZombie->SetValue(SpawnedActor);
//				});
//		});
//	AsyncTask(ENamedThreads::GameThread, [FutureOfZombie = MoveTemp(FutureOfZombie), Callback]()
//		{
//			
//			if (FutureOfZombie.IsReady())
//			{
//				AActor* SpawnedZombie = FutureOfZombie.Get(); 
//				Callback(SpawnedZombie);
//			}
//			else
//			{
//		
//				UE_LOG(LogTemp, Warning, TEXT("Waiting for zombie to spawn..."));
//			}
//		});
//}




void AZSpawn::AllSpawn(int num)
{
	UE_LOG(LogTemp, Warning, TEXT("All Spawn! %d"), IsValidSpawner);
	//SpawnTotal = num;
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, num]() {

		SpawnZombie(num);

	});

}

void AZSpawn::BeginSpawn(int32 InNum)
{
	bCanSpawn = true;
	TotalZombies = InNum;
}




void AZSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bCanSpawn)
	{
		if (TotalZombies <= 0)
		{
			bCanSpawn = false;
			return;
		}
		CurrentSpawnTime += DeltaTime;
		if (CurrentSpawnTime >= SpawnDelay)
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AZEnemy* SpawnedActor = GetWorld()->SpawnActor<AZEnemy>(CubeClass, GetActorLocation() + FVector(0.f, 0.f, 96.f), GetActorRotation(), Params);
			if (SpawnedActor)
			{
				SpawnedActor->SetManager(Manager);
				Manager->EnemyCount += 1;
				TotalZombies--;
			}
			CurrentSpawnTime = 0.f;
		}
	}

	////UE_LOG(LogTemp, Error, TEXT("Name: %s, Valid %d"), *GetName(), IsValidSpawner);
	//if (CurrentlyOverlappingActors.Num() == 0) return;

	//for (AActor* Actor : CurrentlyOverlappingActors)
	//{

	//	if (IsValid(Actor))
	//	{	
	//		Dist = DistanceCalc(Actor->GetActorLocation(), GetActorLocation());

	//		if ((!CheckLos(GetActorLocation(), Actor->GetActorLocation())) && (Dist>400)) {
	//			
	//			IsValidSpawner = true;

	//		}
	//		else {
	//			IsValidSpawner = false;
	//			
	//			break;
	//			
	//		}
	//	}
	//}
	//UE_LOG(LogTemp, Error, TEXT("FOUND %s ACTOR: %d"),*GetName(), IsValidSpawner);

}




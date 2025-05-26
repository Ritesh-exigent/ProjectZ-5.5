// Fill out your copyright notice in the Description page of Project Settings.


#include "ZSpawn.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"

#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Async/Async.h"



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
	if (OtherActor->IsA(CubeClass)) {
		return;

	}


	CurrentlyOverlappingActors.Add(OtherActor);
	IsValidSpawner = false;
	
}

void AZSpawn::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(CubeClass)) {
		return;

	}

	CurrentlyOverlappingActors.Remove(OtherActor);
	IsValidSpawner = true;
	
}



bool AZSpawn::CheckLos(FVector Start, FVector End)
{
	FHitResult HitResult;
	FCollisionQueryParams CParams;
	CParams.AddIgnoredActor(this);

	bool Hit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel1, CParams);
	DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Blue, false, .1f);
	/*if (HitResult.GetActor()->IsA<ATestCharacter>())
	{
	return true;

	}*/
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
	

	float RandomX = FMath::RandRange( - 1.f,  1.f)*MaxSpawningRadius;
	float RandomY = FMath::RandRange(-1.f, 1.f) * MaxSpawningRadius;
	FVector RandomizedLoc(RandomX, RandomY, 0);
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	for (int i = 0; i < NumToSpawn; i++) {
		AsyncTask(ENamedThreads::GameThread, [this, RandomizedLoc, Params]() {
			AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(CubeClass, GetActorLocation() + RandomizedLoc, GetActorRotation(), Params);
			});
	}

	/*SpawnZombieAsync(RandomizedLoc, [](AActor* SpawnedZombie)
		{
			if (SpawnedZombie)
			{
				UE_LOG(LogTemp, Warning, TEXT("Successfully spawned zombie: %s"), *SpawnedZombie->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Zombie spawn failed."));
			}
		});*/

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
	//SpawnTotal = num;
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, num]() {

		SpawnZombie(num);

	});

}




void AZSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (CurrentlyOverlappingActors.Num() == 0) return;

	for (AActor* Actor : CurrentlyOverlappingActors)
	{

		if (IsValid(Actor))
		{	
	
			Dist = DistanceCalc(Actor->GetActorLocation(), GetActorLocation());
			

			if ((!CheckLos(GetActorLocation(), Actor->GetActorLocation())) && (Dist>400)) {
				
				IsValidSpawner = true;

			}
			else {
				IsValidSpawner = false;
				
				break;
				
			}
		}
	}
}




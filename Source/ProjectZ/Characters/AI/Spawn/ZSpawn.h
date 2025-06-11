#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZSpawn.generated.h"

class AZEnemy;
class USphereComponent; 
class UZSpawnManager;
class AZEnemyManager;

UCLASS()
class PROJECTZ_API AZSpawn : public AActor
{
	GENERATED_BODY()

public:
	
	AZSpawn();

	float Dist = 99999999999.f;
	bool IsValidSpawner = true;

	UFUNCTION(BlueprintCallable)
	void AllSpawn(int num);


	void BeginSpawn(int32 InNum);
	__inline void SetManager(AZEnemyManager* InManager) { Manager = InManager; }

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float MaxSpawningRadius = 100.f;

private:

	UPROPERTY(EditAnywhere, Category="Values", meta=(AllowPrivateAccess = true))
	FIntVector2 PoolIDRange;
	UPROPERTY(EditAnywhere, Category="Values", meta=(AllowPrivateAccess = true))
	float SpawnDelay;
	bool bCanSpawn;
	float CurrentSpawnTime;
	int32 TotalZombies;

	int SpawnedCount;
	int SpawnBatchSize;

	AZEnemyManager* Manager;

	UPROPERTY()
	TSet<AActor*> CurrentlyOverlappingActors;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//void SpawnInBatch(int num);
	void SpawnZombie(int NumToSpawn);
	//void SpawnZombieAsync(FVector RandomVector, TFunction<void(AActor*)> Callback);

	float DistanceCalc(FVector Start, FVector End);
	UFUNCTION()
	bool CheckLos(FVector Start, FVector End);


	UFUNCTION()
	void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComp,AActor* OtherActor ,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);


};

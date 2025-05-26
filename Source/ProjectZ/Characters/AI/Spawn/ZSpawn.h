#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZSpawn.generated.h"

class USphereComponent; 
class UZSpawnManager;

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

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	TSubclassOf<AActor> CubeClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float MaxSpawningRadius = 100.f;

private:

	int SpawnedCount;
	int SpawnBatchSize;


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

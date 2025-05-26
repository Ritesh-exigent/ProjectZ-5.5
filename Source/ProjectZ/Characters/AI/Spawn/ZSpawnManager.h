// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ZSpawnManager.generated.h"

class AZSpawn;

UENUM()
enum class  ESpawnType : uint8
{
	Spread,
	Horde,
	Pop,
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTZ_API UZSpawnManager : public UActorComponent
{
	GENERATED_BODY()

public:	

	UZSpawnManager();

	bool bShouldSpawnZombies;
	bool bShouldForceSpawn;

	UFUNCTION(BlueprintCallable)
	void CurateZombieSpawn(int num, ESpawnType NewSpawnType);
	inline void UpdateSpawningParams(bool ShouldSpawn, bool ShouldForceSpawn) { bShouldSpawnZombies = ShouldSpawn; bShouldForceSpawn = ShouldForceSpawn; }



protected:
	
	int CurrentRadiusOfSphere=0;
	int MaxRadiusOfSphere=10000;
	FTimerHandle SpawnHandle;
	FTimerHandle SphereHandle;

	ESpawnType CurrentSpawnType;

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void DistanceSphereTrace();


private:

	int count ;
	int NumberOfZombies;
	int VarienceOfSpawning=3;
	int AmountToSpawn;

	
	TArray<AZSpawn*> FinalSpawners;
	TSet<AZSpawn*> ClosestSpawners;

	void TimedSpawn(TArray<AZSpawn*> ValidSpawners);
	int ScaleByNumber(int num, int OutputRangeCoeff);
	
};

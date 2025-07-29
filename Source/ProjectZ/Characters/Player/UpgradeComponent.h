// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UpgradeComponent.generated.h"

class ASPlayer;

UENUM()
enum EUpgradeStat {
	Health,
	Stamina,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTZ_API UUpgradeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUpgradeComponent();

protected:
	virtual void BeginPlay() override;
private:

	uint32 Points;
	uint32 HealthLevel;
	uint32 StaminaLevel;
	ASPlayer* Player;
		
public:

	void UpgradeLevel(EUpgradeStat InStat);

	inline uint32 GetPoints() { return Points; }

};

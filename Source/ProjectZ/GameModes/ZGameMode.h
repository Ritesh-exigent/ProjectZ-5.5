// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../Quest/QuestUtils.h"
#include "ZGameMode.generated.h"

class ASPlayer;
class AQuestManager;
class AZEnemyManager;

UCLASS()
class PROJECTZ_API AZGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AZGameMode();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	UPROPERTY(EditDefaultsOnly, Category = "Settings | Values")
	TSubclassOf<ASPlayer> PlayerClass;
	UPROPERTY(EditDefaultsOnly, Category = "Settings | Values")
	TSubclassOf<AQuestManager> QuestManagerClass;
	UPROPERTY(EditDefaultsOnly, Category = "Settings | Values")
	TSubclassOf<AZEnemyManager> EnemyManagerClass;
	UPROPERTY(EditDefaultsOnly, Category = "Settings | Values")
	uint8 MaxPlayers;
	UPROPERTY(EditAnywhere, Category = "Settings | Values")
	bool bSpawnEnemies;

	bool bInitZManager;
	TArray<ASPlayer*> Players;

private:

	AZEnemyManager* EnemyManager;
	AQuestManager* QuestManager;

public:

	UFUNCTION(BlueprintCallable)
	AZEnemyManager* GetEnemyManager() { return EnemyManager; }
	__inline TArray<ASPlayer*> GetAllPlayers() { return Players; }
	ASPlayer* GetPlayerPawnAt(int32 Index);
	ASPlayer* GetRandomAlivePlayer();
	FQuestDescription GetCurrentQuestDecription();

};

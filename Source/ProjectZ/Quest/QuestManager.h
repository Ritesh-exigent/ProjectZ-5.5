// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestUtils.h"
#include "QuestManager.generated.h"

class ASPlayer;
class AZGameMode;

UCLASS()
class PROJECTZ_API AQuestManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQuestManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditDefaultsOnly, Category="Values", meta=(AllowPrivateAccess))
	UDataTable* QuestTable;
	

	int32 CurrentSubQuestIndex;
	FQuestInfo* CurrentQuestInfo;
	FSubQuestInfo* CurrentSubQuestInfo;
	TArray<TPair<int32, int32>> SubQuestsData;
	TArray<ASPlayer*> Players;

	FQuestDescription CurrentQuestDescription;
	AZGameMode* ZGameMode;

	void SetCurrentSubQuestData();
	void ActivateQuestItems();

public:

	__inline void SetGameMode(AZGameMode* InGameMode) { ZGameMode = InGameMode; }
	__inline FQuestDescription GetCurrentQuestDescription() { return CurrentQuestDescription; }

	void UpdateQuestData(AQuestItem* Item);

};

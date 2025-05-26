// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "../Quest/QuestUtils.h"
#include "ZGameState.generated.h"

USTRUCT()
struct FEnemyWaveInfo
{
	GENERATED_BODY()

	FEnemyWaveInfo() {
		WaveNum = 0;
		RestTime = 60.f;
	}

	FEnemyWaveInfo(int32 InWaveNum, float InRestTime) {
		WaveNum = InWaveNum;
		RestTime = InRestTime;
	}

	FEnemyWaveInfo& operator= (const FEnemyWaveInfo& RHS)
	{
		if (this == &RHS)
			return *this;

		WaveNum = RHS.WaveNum;
		RestTime = RHS.RestTime;
		return *this;
	}

	UPROPERTY()
	int32 WaveNum;
	UPROPERTY()
	float RestTime;

};

//ENEMYMANAGER DELEGATES................................
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnEnemyInfoUpdated, FEnemyWaveInfo, NewEnemyWaveInfo);

//QUEST DELEGATES........................................
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnQuestUpdated, FText, NewQuestTitle);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSubQuestUpdated, FSubQuestInfo, NewSubQuestInfo);

UCLASS()
class PROJECTZ_API AZGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	AZGameState();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:


	UPROPERTY(ReplicatedUsing = OnRep_EnemyWaveInfo)
	FEnemyWaveInfo EnemyWaveInfo;

public:

	UFUNCTION()
	void OnRep_EnemyWaveInfo();
	void UpdateEnemyWaveInfo(const FEnemyWaveInfo& NewWaveInfo);
	
	FOnEnemyInfoUpdated OnEnemyWaveUpdated;


//QUEST IMPLEMENTATION...........................................

private:

	UPROPERTY(EditDefaultsOnly, Category="Values | Quest", meta=(AllowPrivateAccess = true))
	UDataTable* QuestTable;

	int32 CurrentCount;
	int32 TotalCount;

	bool bStartTimer;
	float TotalTime;

	int32 C_QuestIndex;
	int32 C_SubQuestIndex;
	int32 TotalQuestNumber;

	UPROPERTY(ReplicatedUsing = OnRep_OnCurrentQuestInfoUpdated)
	FQuestInfo C_QuestInfo;
	UPROPERTY(ReplicatedUsing = OnRep_OnCurrentSubQuestInfoUpdated)
	FSubQuestInfo C_SubQuestInfo;

	void StartGameQuest();
	void InitQuestFromDataTable(int32 Index);
	void LoadNextQuest();
	void StartTimer(float InTime);

	void UpdateItemCount(int32 SubQuestID);

	UFUNCTION()
	void OnRep_OnCurrentQuestInfoUpdated();
	UFUNCTION()
	void OnRep_OnCurrentSubQuestInfoUpdated();


public:

	FOnQuestUpdated OnQuestUpdated;
	FOnSubQuestUpdated OnSubQuestUpdated;

	void UpdateQuest(int32 SubQuestID, EQuestItemType ItemType);
};

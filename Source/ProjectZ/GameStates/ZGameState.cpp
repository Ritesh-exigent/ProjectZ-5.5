// Fill out your copyright notice in the Description page of Project Settings.


#include "ZGameState.h"
#include "../Quest/QuestItem.h"
#include "Net/UnrealNetwork.h"

AZGameState::AZGameState()
{
	CurrentCount = 0;
	TotalCount = 0;

	bStartTimer = false;
	TotalTime = 0.f;

	C_QuestInfo = FQuestInfo();
	C_SubQuestInfo = FSubQuestInfo();
}

void AZGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
		StartGameQuest();
}

void AZGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bStartTimer)
	{
		TotalTime -= DeltaTime;
		if (TotalTime <= 0.f)
		{
			TotalTime = 0.f;
			bStartTimer = false;
		}
	}
}

void AZGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AZGameState, CurrentCount);
	DOREPLIFETIME(AZGameState, EnemyWaveInfo);
	DOREPLIFETIME(AZGameState, C_QuestInfo);
	DOREPLIFETIME(AZGameState, C_SubQuestInfo);
}

//ENEMY MANAGER FUNCTIONS............................

void AZGameState::OnRep_EnemyWaveInfo()
{
	OnEnemyWaveUpdated.ExecuteIfBound(EnemyWaveInfo);
}

void AZGameState::UpdateEnemyWaveInfo(const FEnemyWaveInfo& NewWaveInfo)
{
	EnemyWaveInfo = NewWaveInfo;
	OnRep_EnemyWaveInfo();
}

//QUEST FUNCTIONS.....................................

void AZGameState::StartGameQuest()
{
	TArray<FQuestInfo*> OutArray;
	QuestTable->GetAllRows<FQuestInfo>("", OutArray);
	TotalQuestNumber = OutArray.Num();

	C_QuestIndex = 1;
	InitQuestFromDataTable(C_QuestIndex);
}

void AZGameState::InitQuestFromDataTable(int32 Index)
{
	FString RowName = "Q";
	RowName += FString::FromInt(C_QuestIndex);
	FQuestInfo* CurrentQuest = QuestTable->FindRow<FQuestInfo>(FName(RowName), "");
	if (CurrentQuest)
	{
		C_QuestInfo = *CurrentQuest;
		if (CurrentQuest->SubQuests.Num() > 0)
		{
			C_SubQuestIndex = 0;
			C_SubQuestInfo = CurrentQuest->SubQuests[C_SubQuestIndex];
			ActivateSubQuestItems(C_SubQuestInfo.RequiredItems);
			TotalCount = C_SubQuestInfo.TotalCount;
			TotalTime = C_SubQuestInfo.TotalTime;
			OnRep_OnCurrentQuestInfoUpdated();
			OnRep_OnCurrentSubQuestInfoUpdated();
		}
	}
}

void AZGameState::LoadNextQuest()
{
	++C_SubQuestIndex;
	if (C_SubQuestIndex >= C_QuestInfo.SubQuests.Num())
	{
		++C_QuestIndex;
		if (C_QuestIndex > TotalQuestNumber)
		{
			//GameFinished
			return;
		}
		InitQuestFromDataTable(C_QuestIndex);
	}
	else
	{
		C_SubQuestInfo = C_QuestInfo.SubQuests[C_SubQuestIndex];
		ActivateSubQuestItems(C_SubQuestInfo.RequiredItems);
		TotalCount = C_SubQuestInfo.TotalCount;
		TotalTime = C_SubQuestInfo.TotalTime;
		OnRep_OnCurrentSubQuestInfoUpdated();
	}
}

void AZGameState::Server_StartGameQuest_Implementation()
{
	StartGameQuest();
}

void AZGameState::OnRep_OnCurrentCountUpdated()
{
	OnQuestItemCountUpdated.ExecuteIfBound(CurrentCount);
}

void AZGameState::OnRep_OnCurrentQuestInfoUpdated()
{
	OnQuestUpdated.ExecuteIfBound(C_QuestInfo.QuestTitle);
}

void AZGameState::OnRep_OnCurrentSubQuestInfoUpdated()
{
	OnSubQuestUpdated.ExecuteIfBound(C_SubQuestInfo);
}

void AZGameState::UpdateQuest(int32 SubQuestID, EQuestItemType ItemType)
{
	if (C_SubQuestInfo.SubQuestID == SubQuestID)
	{
		if (C_SubQuestInfo.SubQuestType == EQuestType::QT_Collect)
			UpdateItemCount(SubQuestID);
		else //if (C_SubQuestInfo.SubQuestType == EQuestType::QT_Time)
		{
			LoadNextQuest();
		}
	}
}

void AZGameState::UpdateCurrentQuest()
{
	UpdateQuest(C_SubQuestInfo.SubQuestID, EQuestItemType::Interactable);
}

void AZGameState::UpdateItemCount(int32 SubQuestID)
{
	if (C_SubQuestInfo.SubQuestID == SubQuestID)//redundant
	{
		CurrentCount++;
		OnRep_OnCurrentCountUpdated();
		if (CurrentCount == TotalCount)
		{
			//finish quest
			LoadNextQuest();
		}
	}

}

void AZGameState::ActivateSubQuestItems(TArray<TSoftObjectPtr<AQuestItem>>& RequiredItems)
{
	if (RequiredItems.Num() <= 0) return;

	for (TSoftObjectPtr<AQuestItem> Item : RequiredItems)
	{
		if (Item.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Item Name: %s"), *Item->GetName());
			Item->Activate();
		}
	}
}

void AZGameState::StartTimer(float InTime)
{
	bStartTimer = true;
	TotalTime = InTime;
}

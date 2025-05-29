// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestManager.h"
#include "QuestItem.h"
#include "../GameModes/ZGameMode.h"
#include "../Characters/Player/SPlayer.h"


// Sets default values
AQuestManager::AQuestManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	QuestTable = nullptr;
	CurrentQuestInfo = nullptr;
	CurrentSubQuestInfo = nullptr;
	CurrentSubQuestIndex = 0;
}

// Called when the game starts or when spawned
void AQuestManager::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentQuestInfo = QuestTable->FindRow<FQuestInfo>(FName("Q1"), "");
	SetCurrentSubQuestData();
}

// Called every frame
void AQuestManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AQuestManager::SetCurrentSubQuestData()
{
	if (!CurrentQuestInfo)
		return;

	CurrentSubQuestInfo = &CurrentQuestInfo->SubQuests[CurrentSubQuestIndex];

	CurrentQuestDescription.QTitle = CurrentQuestInfo->QuestTitle;
	CurrentQuestDescription.SQTitle = CurrentSubQuestInfo->SubQuestTitle;
	if (CurrentSubQuestInfo->SubQuestType == EQuestType::QT_Collect)
		CurrentQuestDescription.TotalCount = FQuestDescription::IntToText(CurrentSubQuestInfo->TotalCount);
	else if (CurrentSubQuestInfo->SubQuestType == EQuestType::QT_Time)
		CurrentQuestDescription.TotalCount = FQuestDescription::FloatToText(CurrentSubQuestInfo->TotalTime);

}

void AQuestManager::ActivateQuestItems()
{
	/*for (FSubQuestInfo& SubInfo : CurrentQuestInfo->SubQuests)
	{
		for (AQuestItem* Item : SubInfo.RequiredItems)
		{
			Item->Activate();
		}
	}*/
}

void AQuestManager::UpdateQuestData(AQuestItem* Item)
{

}


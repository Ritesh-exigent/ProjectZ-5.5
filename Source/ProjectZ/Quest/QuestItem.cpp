// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestItem.h"
#include "../GameStates/ZGameState.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AQuestItem::AQuestItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bIsActive = false;
}

// Called when the game starts or when spawned
void AQuestItem::BeginPlay()
{
	Super::BeginPlay();
	ZGameState = GetWorld()->GetGameState<AZGameState>();
}

// Called every frame
void AQuestItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AQuestItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AQuestItem, bIsActive);
}

void AQuestItem::Server_SetItemActive_Implementation(bool bValue)
{
	bIsActive = bValue;
}

void AQuestItem::Activate()
{
	bIsActive = true;
}

void AQuestItem::Deactivate()
{
	bIsActive = false;
}

void AQuestItem::Interact(ASPlayer* InPlayer)
{
	if (bIsActive && ZGameState)
	{
		UE_LOG(LogTemp, Warning, TEXT("Quest Interact triggered"));
		ZGameState->UpdateQuest(SubQuestID, ItemType);
		Deactivate();
		if (ItemType == EQuestItemType::Pickup)
			Destroy();
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestItem.h"
#include "Net/UnrealNetwork.h"
#include "../GameStates/ZGameState.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../GameModes/ZGameMode.h"


// Sets default values
AQuestItem::AQuestItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bIsInteracted = false;
	bIsActive = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	Mesh->SetIsReplicated(true);
	SetRootComponent(Mesh);

	MarkerWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	MarkerWidgetComponent->SetIsReplicated(true);
	MarkerWidgetComponent->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void AQuestItem::BeginPlay()
{
	Super::BeginPlay();
	ZGameState = GetWorld()->GetGameState<AZGameState>();
}

void AQuestItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AQuestItem, bIsActive);
	DOREPLIFETIME(AQuestItem, bIsInteracted);
}

void AQuestItem::OnRep_IsInteracted()
{
	OnInteract();
}

void AQuestItem::OnRep_IsQuestActive()
{
	Mesh->SetRenderCustomDepth(bIsActive);
	if (bIsActive)
		MarkerWidgetComponent->GetWidget()->SetVisibility(ESlateVisibility::Visible);
	else
		MarkerWidgetComponent->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
}

void AQuestItem::Client_OnInteract_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Client door triggered!"));
	OnInteract();
}

void AQuestItem::Activate()
{
	if (HasAuthority())
	{
		bIsActive = true;
		OnRep_IsQuestActive();
	}
}

void AQuestItem::Deactivate()
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Quest Deactivated"));
		bIsActive = false;
		OnRep_IsQuestActive();
	}
}

void AQuestItem::Interact(ASPlayer* InPlayer)
{
	if (HasAuthority())
	{
		if (bIsActive && ZGameState && ZGameState->IsWaveCompleted())
		{
			UE_LOG(LogTemp, Warning, TEXT("Quest Interact triggered"));
			ZGameState->UpdateQuest(SubQuestID, ItemType);
			Deactivate();

			if (bCanSpawnEnemies)
			{
				AZGameMode* GM = Cast<AZGameMode>(GetWorld()->GetAuthGameMode());
				if (GM)
				GM->InitEnemies();
			}

			bIsInteracted = true;
			OnRep_IsInteracted();

			if (ItemType == EQuestItemType::Pickup)
				Destroy();
		}
	}
}

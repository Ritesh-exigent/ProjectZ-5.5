// Fill out your copyright notice in the Description page of Project Settings.


#include "ZGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "./ProjectZ/GameStates/ZGameState.h"
#include "./ProjectZ/Characters/Player/SPlayer.h"
#include "./ProjectZ/Characters/AI/Manager/ZEnemyManager.h"

AZGameMode::AZGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	GameStateClass = AZGameState::StaticClass();



	MaxPlayers = 3;
	bInitZManager = false;
	bSpawnEnemies = false;
}

void AZGameMode::BeginPlay()
{

	EnemyManager = GetWorld()->SpawnActor<AZEnemyManager>(EnemyManagerClass);
	/*if (EnemyManager)
		EnemyManager->InitEnemies();*/
}

void AZGameMode::Tick(float DeltaTime)
{
}

void AZGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GetNumPlayers() > MaxPlayers)
	{
		NewPlayer->ClientReturnToMainMenuWithTextReason(FText::FromString("Player limit reached"));
		return;
	}

	//to be changed later
	AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());
	if (PlayerStart)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(PlayerStart->GetActorLocation());
		SpawnTransform.SetRotation(PlayerStart->GetActorRotation().Quaternion());
		SpawnTransform.SetScale3D(FVector::OneVector);
		if (ASPlayer* Player = GetWorld()->SpawnActor<ASPlayer>(PlayerClass, SpawnTransform))
		{
			NewPlayer->Possess(Player);
			Players.Add(Player);
			
			if (!bInitZManager && EnemyManager && bSpawnEnemies)
			{
				EnemyManager->InitEnemies();
				bInitZManager = true;
			}
		}
	}

}

ASPlayer* AZGameMode::GetPlayerPawnAt(int32 Index)
{
	if(Index >= Players.Num() || Index < 0)
	return nullptr;
	return Players[Index];
}

ASPlayer* AZGameMode::GetRandomAlivePlayer()
{
	for (ASPlayer* Player : Players)
	{
		if (Player)
		{
			if (!Player->IsDead())
			{

			}
		}
	}
	return nullptr;
}

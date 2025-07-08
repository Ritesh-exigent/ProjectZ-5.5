// Fill out your copyright notice in the Description page of Project Settings.


#include "ZPlayerController.h"
#include "SPlayer.h"
#include "ProjectZ/Interfaces/ZPawnInterface.h"

AZPlayerController::AZPlayerController()
{
	bReplicates = true;
	Player = nullptr;
}

void AZPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AZPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (!Player)
		Player = Cast<ASPlayer>(InPawn);
}

void AZPlayerController::Server_SwitchPawn_Implementation(APawn* InNewPawn)
{
	PossessPawn(InNewPawn);
}

void AZPlayerController::SwitchPawn(APawn* InNewPawn)
{
	if (HasAuthority())
		PossessPawn(InNewPawn);
	else
		Server_SwitchPawn(InNewPawn);
}

void AZPlayerController::PossessPawn(APawn* InPawn)
{
	UnPossess();

	if (InPawn)
		Possess(InPawn);
	else
		Possess(Player);
}

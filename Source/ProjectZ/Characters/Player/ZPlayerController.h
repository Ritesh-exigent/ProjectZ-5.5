// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ZPlayerController.generated.h"

class ASPlayer;

UCLASS()
class PROJECTZ_API AZPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	
public:

	AZPlayerController();

protected:

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	ASPlayer* Player;

	void PossessPawn(APawn* InPawn);

	UFUNCTION(Server, Reliable)
	void Server_SwitchPawn(APawn* InNewPawn);
	void Server_SwitchPawn_Implementation(APawn* InNewPawn);


public:

	UFUNCTION(BlueprintCallable)
	void SwitchPawn(APawn* InNewPawn);
};

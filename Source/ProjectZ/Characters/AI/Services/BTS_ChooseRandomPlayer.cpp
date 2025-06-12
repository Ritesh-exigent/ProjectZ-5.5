// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_ChooseRandomPlayer.h"
#include "../Controllers/ZAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_ChooseRandomPlayer::UBTS_ChooseRandomPlayer()
{
	NodeName = TEXT("ChooseRandomPlayer");
	CurrentTime = RecheckCooldown;
}

void UBTS_ChooseRandomPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{	
	CurrentTime += DeltaSeconds;
	if (CurrentTime >= RecheckCooldown)
	{
		SelectRandomPlayer(OwnerComp);
		CurrentTime = 0.f;
	}
	
}

void UBTS_ChooseRandomPlayer::SelectRandomPlayer(UBehaviorTreeComponent& OwnerComp)
{
	int32 NumPlayers = GetWorld()->GetAuthGameMode()->GetNumPlayers();
	int32 RandomIndex = FMath::RandRange(0, NumPlayers - 1);
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), UGameplayStatics::GetPlayerPawn(GetWorld(), RandomIndex));
}

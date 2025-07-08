// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_ChooseRandomPlayer.h"
#include "../Controllers/ZAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "./ProjectZ/GameModes/ZGameMode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "./ProjectZ/Characters/Player/SPlayer.h"

UBTS_ChooseRandomPlayer::UBTS_ChooseRandomPlayer()
{
	NodeName = TEXT("ChooseRandomPlayer");
	CurrentTime = RecheckCooldown;
}

void UBTS_ChooseRandomPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{	
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	CurrentTime += DeltaSeconds;
	if (CurrentTime >= RecheckCooldown)
	{
		SelectRandomPlayer(OwnerComp);
		CurrentTime = 0.f;
	}
}

FString UBTS_ChooseRandomPlayer::GetStaticDescription() const
{
	return FString::Printf(TEXT("Current Time: %f"), CurrentTime);
}

void UBTS_ChooseRandomPlayer::SelectRandomPlayer(UBehaviorTreeComponent& OwnerComp)
{
	ZGameMode = Cast<AZGameMode>(GetWorld()->GetAuthGameMode());
	if (ZGameMode)
	{
		int32 NumPlayers = ZGameMode->GetNumPlayers();
		int32 RandomIndex = FMath::RandRange(0, NumPlayers - 1);
		APawn* Pawn = Cast<APawn>(ZGameMode->GetPlayerPawnAt(RandomIndex));
		if(Pawn)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), Pawn);
			//UE_LOG(LogTemp, Warning, TEXT("New Player Set: %s , for: %s"), *Pawn->GetName(), *OwnerComp.GetAIOwner()->GetPawn()->GetName());
		}
		/*else
			UE_LOG(LogTemp, Warning, TEXT("New Player selected was nullptr , for: %s"), *OwnerComp.GetAIOwner()->GetPawn()->GetName());*/

	}
}

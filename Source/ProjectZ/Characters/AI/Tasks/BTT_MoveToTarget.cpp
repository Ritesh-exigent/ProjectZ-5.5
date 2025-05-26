// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MoveToTarget.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectZ/GameModes/ZGameMode.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectZ/Characters/Player/SPlayer.h"
#include "ProjectZ/Characters/AI/Controllers/ZAIController.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

UBTT_MoveToTarget::UBTT_MoveToTarget()
{
	NodeName = "Z_MoveToTarget";
	bNotifyTick = true;
	TargetLocation = FVector::ZeroVector;
}

void UBTT_MoveToTarget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(UBTT_MoveToTarget, PlayerPawn);
}

EBTNodeResult::Type UBTT_MoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ZController = Cast<AZAIController>(OwnerComp.GetAIOwner());
	ZGameMode = Cast<AZGameMode>(OwnerComp.GetWorld()->GetAuthGameMode());
	/*if (!ZController || !ZGameMode)
		return EBTNodeResult::Failed;*/



	bMoveFinish = false;

	if (bChasePlayer)
	{
		PlayerPawn = ZGameMode->GetPlayerPawnAt(0);
		//UE_LOG(LogTemp, Warning, TEXT("Player: %s"), *PlayerPawn->GetName());
		TargetLocation = PlayerPawn->GetActorLocation();
		PreviousLocation = TargetLocation;
	}
	else
	TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TargetKey.SelectedKeyName);
	//UE_LOG(LogTemp, Warning, TEXT("%s : Recieved TargetLocation: %s"), *ZController->GetPawn()->GetName(), *TargetLocation.ToString());
	InitMove();
	/*if (OwnerComp.GetOwner()->HasAuthority())
		InitMove();
	else
		Server_InitMove();*/
	
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTT_MoveToTarget::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(!ZController)
		ZController = Cast<AZAIController>(OwnerComp.GetAIOwner());
	else
	{
		ZController->AbortAIMovement();
	}
	return EBTNodeResult::Aborted;
}

void UBTT_MoveToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaTime)
{
	//update the pathpoints
	//traverse the path
	UpdateMove();
	/*if (OwnerComp.GetOwner()->HasAuthority())
		UpdateMove();
	else
		Server_UpdateMove();*/

	if (bMoveFinish)
		return FinishLatentTask(OwnerComp, MoveResult);
}

//FVector UBTT_MoveToTarget::SelectRandomPlayerLocation()
//{
//	if (ZGameMode)
//	{
//		int32 RandomPlayerNum = FMath::RandRange(0, ZGameMode->GetNumPlayers()-1);
//		ASPlayer* PlayerPawn = ZGameMode->GetPlayerPawnAt(RandomPlayerNum);
//		if(PlayerPawn)
//		return PlayerPawn->GetActorLocation();
//	}
//	return FVector::ZeroVector;
//}
//
//FVector UBTT_MoveToTarget::GetClosestPlayerLocation(UBehaviorTreeComponent& OwnerComp)
//{
//	int32 NumPlayers = ZGameMode->GetNumPlayers();
//	int32 SelectedPlayer = 0;
//	for (uint8 i = 1; i < (uint8)NumPlayers; ++i)
//	{
//		if (CheckDistance(OwnerComp, SelectedPlayer) < CheckDistance(OwnerComp, i))
//			SelectedPlayer = i;
//	}
//	ASPlayer* PlayerPawn = ZGameMode->GetPlayerPawnAt(SelectedPlayer);
//	if(PlayerPawn)
//	return PlayerPawn->GetActorLocation();
//	return FVector::ZeroVector;
//}
//
//double UBTT_MoveToTarget::CheckDistance(UBehaviorTreeComponent& OwnerComp, int32 Index)
//{
//	APawn* PlayerPawn = ZGameMode->GetPlayerPawnAt(Index);
//	if(PlayerPawn)
//	return FVector::Dist(OwnerComp.GetOwner()->GetActorLocation(), PlayerPawn->GetActorLocation());
//	return 10e5;
//}

void UBTT_MoveToTarget::Server_InitMove_Implementation()
{
	InitMove();
}

void UBTT_MoveToTarget::InitMove()
{
	if (TargetLocation != FVector::ZeroVector)
	{
		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		if (NavSystem)
		{
			UNavigationPath* Path = NavSystem->FindPathToLocationSynchronously(GetWorld(), ZController->GetPawn()->GetActorLocation(), TargetLocation);
			if (Path && Path->PathPoints.Num() > 0)
			{
				PathPoints = Path->PathPoints;
				CurrentPathIndex = 1;
				CurrentPathPoint = PathPoints[CurrentPathIndex];
				//if(!bChasePlayer)
				CurrentPathPoint.Z += 96.f;
				ZController->RequestAIMovement(CurrentPathPoint, EMoveState::Walk, AcceptRadius);

			}
		}
	}
}

void UBTT_MoveToTarget::Server_UpdateMove_Implementation()
{
	UpdateMove();
}

void UBTT_MoveToTarget::UpdateMove()
{
	//UE_LOG(LogTemp, Warning, TEXT("%s: TargetLocation: %s"), *ZController->GetPawn()->GetName(), *TargetLocation.ToString());
	if (PathPoints.Num() == 0)
	{
		bMoveFinish = true;
		MoveResult = EBTNodeResult::Failed;
	}

	if (ZController->IsAIMovementCompleted())
	{
		CurrentPathIndex++;
		if (CurrentPathIndex >= PathPoints.Num())
		{
			bMoveFinish = true;
			MoveResult = EBTNodeResult::Succeeded;
		}
		else
		{
			CurrentPathPoint = PathPoints[CurrentPathIndex];
			//if(!bChasePlayer)
			CurrentPathPoint.Z += 96.f;
			ZController->RequestAIMovement(CurrentPathPoint, EMoveState::Walk, AcceptRadius);
		}
	}
	if(bChasePlayer && PlayerPawn->GetActorLocation() != PreviousLocation)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Chasing Player"));
		PreviousLocation = PlayerPawn->GetActorLocation();
		TargetLocation = PreviousLocation;
		InitMove();
	}
}

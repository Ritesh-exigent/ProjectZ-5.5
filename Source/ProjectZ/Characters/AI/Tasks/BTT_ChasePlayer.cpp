// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ChasePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "ProjectZ/Characters/AI/Controllers/ZAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectZ/Characters/Player/SPlayer.h"

UBTT_ChasePlayer::UBTT_ChasePlayer()
{
	NodeName = "ChasePlayer";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ZController = Cast<AZAIController>(OwnerComp.GetAIOwner());
	if (!ZController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller is nullptr"));
		return EBTNodeResult::Failed;
	}
	TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));

	PlayerPawn = Cast<ASPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if(!TargetActor)//!PlayerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetActor is nullptr"));
		return EBTNodeResult::Failed;
	}

	PreviousLocation = TargetActor->GetActorLocation();//PlayerPawn->GetActorLocation();

	if (FVector::Dist(TargetActor->GetActorLocation()/*PlayerPawn->GetActorLocation() */ , ZController->GetPawn()->GetActorLocation()) <= AcceptRadius)
		return EBTNodeResult::Succeeded;

	if (CalcPath())
	{
		ZController->RequestAIMovement(CurrentPathPoint, EMoveState::Walk, AcceptRadius);
		return EBTNodeResult::InProgress;
	}
	UE_LOG(LogTemp, Warning, TEXT("NavSys is null"));
	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTT_ChasePlayer::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!ZController)
		ZController = Cast<AZAIController>(OwnerComp.GetAIOwner());
	else
	{
		ZController->AbortAIMovement();
	}
	return EBTNodeResult::Aborted;
}

void UBTT_ChasePlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaTime)
{
	if (!TargetActor)//!PlayerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetActor is nullptr"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
	}

	if (FVector::Dist(TargetActor->GetActorLocation()/*PlayerPawn->GetActorLocation()*/, ZController->GetPawn()->GetActorLocation()) > AcceptRadius && PreviousLocation != TargetActor->GetActorLocation())
	{
		if (!CalcPath())
			FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
	}
	else
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	if (ZController->IsAIMovementCompleted())
	{
		CurrentPathIndex++;
		CurrentPathPoint = PathPoints[CurrentPathIndex];
		//CurrentPathPoint.Z += 96.f;
		ZController->RequestAIMovement(CurrentPathPoint, EMoveState::Walk, AcceptRadius);
		//FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

bool UBTT_ChasePlayer::CalcPath()
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (NavSystem)
	{
		UNavigationPath* Path = NavSystem->FindPathToLocationSynchronously(GetWorld(), ZController->GetPawn()->GetActorLocation(), PreviousLocation);
		if (Path && Path->PathPoints.Num() > 0)
		{
			PathPoints = Path->PathPoints;
			CurrentPathIndex = 1;
			CurrentPathPoint = PathPoints[CurrentPathIndex];
			CurrentPathPoint.Z += 96.f;
			//ZController->RequestAIMovement(CurrentPathPoint, NMoveStatus::EMoveState::Walk, AcceptRadius);
			return true;
		}
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("NavSys is nullptr"));
	return false;
}

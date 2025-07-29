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
	HalfHeight = 0.f;
	Count = 0;
}

EBTNodeResult::Type UBTT_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FChaseMemory* ChaseMemory = (FChaseMemory*)NodeMemory;
	ChaseMemory->ZController = Cast<AZAIController>(OwnerComp.GetAIOwner());
	if (!(ChaseMemory->ZController))
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller is nullptr"));
		return EBTNodeResult::Failed;
	}
	ChaseMemory->HalfHeight = ChaseMemory->ZController->GetPawn()->GetDefaultHalfHeight();
	ChaseMemory->TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));

	if(!(ChaseMemory->TargetActor))
	{
		return EBTNodeResult::Failed;
	}

	ChaseMemory->PreviousLocation = ChaseMemory->TargetActor->GetActorLocation();



	if (FVector::Dist(ChaseMemory->TargetActor->GetActorLocation()  , ChaseMemory->ZController->GetPawn()->GetActorLocation()) <= AcceptRadius)
		return EBTNodeResult::Succeeded;

	if (CalcPath(ChaseMemory))
	{
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTT_ChasePlayer::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FChaseMemory* ChaseMemory = (FChaseMemory*)NodeMemory;
	if (ChaseMemory)
	{
		if (!(ChaseMemory->ZController))
			ChaseMemory->ZController = Cast<AZAIController>(OwnerComp.GetAIOwner());
		else
		{
			ChaseMemory->ZController->AbortAIMovement();
		}
	}
	return EBTNodeResult::Aborted;
}

void UBTT_ChasePlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaTime)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaTime);

	FChaseMemory* ChaseMemory = (FChaseMemory*)NodeMemory;

	if ((ChaseMemory && !ChaseMemory->TargetActor) || !ChaseMemory)
	{
		FinishLatentAbort(OwnerComp);
		return;
	}
	
	if (ChaseMemory->ZController->IsAIMovementCompleted())
	{
		ChaseMemory->CurrentPathIndex++;
		if (ChaseMemory->CurrentPathIndex >= ChaseMemory->PathPoints.Num())
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
		else
		{
			FVector CurrentPathPoint = ChaseMemory->PathPoints[ChaseMemory->CurrentPathIndex];
			CurrentPathPoint.Z += ChaseMemory->HalfHeight;
			ChaseMemory->ZController->RequestAIMovement(CurrentPathPoint, EMoveState::Walk, AcceptRadius);
		}
	}
	
	if (ChaseMemory->PreviousLocation != ChaseMemory->TargetActor->GetActorLocation())
	{
		ChaseMemory->PreviousLocation = ChaseMemory->TargetActor->GetActorLocation();
		if (!CalcPath(ChaseMemory))
		{
			FinishLatentAbort(OwnerComp);
		}
	}

	//not an efficient fix, todo later....
	//FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

uint16 UBTT_ChasePlayer::GetInstanceMemorySize() const
{
	return sizeof(FChaseMemory);
}

bool UBTT_ChasePlayer::CalcPath(FChaseMemory* ChaseMemory)
{
	if (!ChaseMemory)
		return false;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem)
	{
		UNavigationPath* Path = NavSystem->FindPathToLocationSynchronously(GetWorld(), ChaseMemory->ZController->GetPawn()->GetActorLocation(), ChaseMemory->TargetActor->GetActorLocation());
		if (Path && Path->PathPoints.Num() > 0)
		{
			ChaseMemory->PathPoints = Path->PathPoints;
			ChaseMemory->CurrentPathIndex = 1;
			FVector CurrentPathPoint = ChaseMemory->PathPoints[ChaseMemory->CurrentPathIndex];
			CurrentPathPoint.Z += ChaseMemory->HalfHeight;
			ChaseMemory->ZController->RequestAIMovement(CurrentPathPoint, EMoveState::Walk, AcceptRadius);
			return true;
		}
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("NavSys is nullptr"));
	return false;
}

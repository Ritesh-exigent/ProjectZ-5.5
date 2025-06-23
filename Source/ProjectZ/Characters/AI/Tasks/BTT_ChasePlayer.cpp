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
}

EBTNodeResult::Type UBTT_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ZController = Cast<AZAIController>(OwnerComp.GetAIOwner());
	if (!ZController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller is nullptr"));
		return EBTNodeResult::Failed;
	}
	HalfHeight = ZController->GetPawn()->GetDefaultHalfHeight();
	/*if (bForceChase)
	{
		int32 NumPlayers = GetWorld()->GetNumPlayerControllers();
		int32 RandomPlayer = FMath::RandRange(0, NumPlayers-1);
		TargetActor = UGameplayStatics::GetPlayerPawn(GetWorld(), RandomPlayer);
	}
	else*/
	TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));
	//UE_LOG(LogTemp, Warning, TEXT("Execute Tick Test"));
	//PlayerPawn = Cast<ASPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if(!TargetActor)//!PlayerPawn)
	{
		//UE_LOG(LogTemp, Warning, TEXT("TargetActor is nullptr"));
		return EBTNodeResult::Failed;
	}

	PreviousLocation = TargetActor->GetActorLocation();//PlayerPawn->GetActorLocation();
		//UE_LOG(LogTemp, Warning, TEXT("PreviousLocation :%s"), *PreviousLocation.ToString());


	if (FVector::Dist(TargetActor->GetActorLocation()/*PlayerPawn->GetActorLocation() */ , ZController->GetPawn()->GetActorLocation()) <= AcceptRadius)
		return EBTNodeResult::Succeeded;

	if (CalcPath())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Task InProgress"));
		//Count = 0;
		//ZController->RequestAIMovement(CurrentPathPoint, EMoveState::Walk, AcceptRadius);
		return EBTNodeResult::InProgress;
	}
	//else
	//UE_LOG(LogTemp, Warning, TEXT("Failed to find path"));

	//UE_LOG(LogTemp, Warning, TEXT("Task Failed"));
	//UE_LOG(LogTemp, Warning, TEXT("NavSys is null"));
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
	Super::TickTask(OwnerComp, NodeMemory, DeltaTime);
	//Count++;
	//UE_LOG(LogTemp, Warning, TEXT("Chase Tick Working! Count: %d"), Count);
	if (!TargetActor)//!PlayerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetActor is nullptr"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
		return;
	}

	//if (FVector::Dist(TargetActor->GetActorLocation()/*PlayerPawn->GetActorLocation()*/, ZController->GetPawn()->GetActorLocation()) > AcceptRadius && PreviousLocation != TargetActor->GetActorLocation())
	//{
	//	if (!CalcPath())
	//		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
	//	else
	//		ZController->RequestAIMovement(CurrentPathPoint, EMoveState::Walk, AcceptRadius);
	//}
	
	if (ZController->IsAIMovementCompleted())
	{
		CurrentPathIndex++;
		if (CurrentPathIndex >= PathPoints.Num())
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
		else
		{
			CurrentPathPoint = PathPoints[CurrentPathIndex];
			CurrentPathPoint.Z += HalfHeight;
			//UE_LOG(LogTemp, Warning, TEXT("CurrentPathPoint: %s"), *CurrentPathPoint.ToString());
			DrawDebugSphere(GetWorld(), CurrentPathPoint, 50.f, 3, FColor::Green, false, 5.f);
			ZController->RequestAIMovement(CurrentPathPoint, EMoveState::Walk, AcceptRadius);
		}
	}
	
	if (PreviousLocation != TargetActor->GetActorLocation())
	{
		PreviousLocation = TargetActor->GetActorLocation();
		//UE_LOG(LogTemp, Warning, TEXT("Previous not equal"));
		if (!CalcPath())
			FinishLatentAbort(OwnerComp);
	}

	//FinishLatentTask(OwnerComp, EBTNodeResult::InProgress);
}

bool UBTT_ChasePlayer::CalcPath()
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld()); //GetNavigationSystem(GetWorld());
	if (NavSystem)
	{
		UNavigationPath* Path = NavSystem->FindPathToLocationSynchronously(GetWorld(), ZController->GetPawn()->GetActorLocation(), PreviousLocation);
		if (Path && Path->PathPoints.Num() > 0)
		{
			PathPoints = Path->PathPoints;
			CurrentPathIndex = 1;
			CurrentPathPoint = PathPoints[CurrentPathIndex];
			CurrentPathPoint.Z += HalfHeight;
			//UE_LOG(LogTemp, Warning, TEXT("FoundPath!"));
			ZController->RequestAIMovement(CurrentPathPoint, EMoveState::Walk, AcceptRadius);
			return true;
		}
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("NavSys is nullptr"));
	return false;
}

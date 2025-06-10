// Fill out your copyright notice in the Description page of Project Settings.


#include "ZAIController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "./ProjectZ/Characters/AI/ZEnemy.h"
#include "../Perception/PerceptionComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectZ/Characters/MoveComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

AZAIController::AZAIController()
{
	//BTComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	//BBComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlacboardComponent"));
}

void AZAIController::BeginPlay()
{
	Super::BeginPlay();
	bIsTargetDetected = false;
	SetActorTickEnabled(false);
}

void AZAIController::OnPossess(APawn* InPawn)
{	
	Super::OnPossess(InPawn);
	Z_Pawn = Cast<AZEnemy>(InPawn);
	if (Z_Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is possessed by %s"), *Z_Pawn->GetName(), *GetName());
		Z_Pawn->GetPerceptionComponent()->OnTargetFound.AddDynamic(this, &AZAIController::OnTargetFound);
		UBlackboardComponent* ZBB = nullptr;
		if (BTree && UseBlackboard(BTree->BlackboardAsset, ZBB))
		{
			Blackboard = ZBB;
			RunBehaviorTree(BTree);
			UE_LOG(LogTemp, Warning, TEXT("BTree Name: %s , Blackboard Name: %s"), *BTree->GetName(), *Blackboard->GetName());
			ZBB = nullptr;
			delete(ZBB);
		}
	}
}

void AZAIController::OnUnPossess()
{
	StopBehaviorTree();
}

void AZAIController::RequestAIMovement(FVector InTargetLocation, EMoveState InState, float InAcceptanceRadius)
{
	if (Z_Pawn)
	{
		Z_Pawn->GetMoveComponent()->RequestAIMovement(InTargetLocation, InState, InAcceptanceRadius);
		//DrawDebugSphere(GetWorld(), InTargetLocation, 50.f, 4, FColor::Red, false, 5.f);
		//UE_LOG(LogTemp, Warning, TEXT("%s : Request Location: %s"), *GetName(), *InTargetLocation.ToString());
	}
}

//void AZAIController::RequestAIMovement_Blueprint(FVector InTargetLocation, EBMoveState InState, float InAcceptanceRadius)
//{
//	if(InState == EBMoveState::Walk)
//	RequestAIMovement(InTargetLocation, EMoveState::Walk, InAcceptanceRadius);
//	else
//	RequestAIMovement(InTargetLocation, EMoveState::Run, InAcceptanceRadius);
//
//}

void AZAIController::AbortAIMovement()
{
	if (Z_Pawn)
		Z_Pawn->GetMoveComponent()->AbortAIMovement();
}

bool AZAIController::IsAIMovementCompleted()
{
	if (Z_Pawn)
		return Z_Pawn->GetMoveComponent()->IsAIMovementCompleted();
	return false;
}

void AZAIController::PauseMovement(bool InValue)
{
	if (Z_Pawn)
		Z_Pawn->GetMoveComponent()->PauseMovement(InValue);
}

void AZAIController::Attack(bool bOverride)
{
	if(Z_Pawn)
	Z_Pawn->Attack(bOverride);
}

void AZAIController::StopBehaviorTree()
{
	UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComp)
	{
		BTComp->StopTree();
		UE_LOG(LogTemp, Warning, TEXT("BT Stopped"));
	}
}

void AZAIController::OnTargetFound(AActor* InTargetActor)
{
	bIsTargetDetected = true;
	TargetActor = InTargetActor;
}
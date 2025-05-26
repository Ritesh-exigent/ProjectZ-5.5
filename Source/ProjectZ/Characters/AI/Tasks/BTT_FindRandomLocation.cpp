// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_FindRandomLocation.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

UBTT_FindRandomLocation::UBTT_FindRandomLocation()
{
	NodeName = TEXT("Z_FindRandomLocation");
}

void UBTT_FindRandomLocation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(UBTT_FindRandomLocation, RandomLocation);
}

EBTNodeResult::Type UBTT_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{ 
	//UE_LOG(LogTemp, Warning, TEXT("Current Blackboard: %s"), *OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetName())
	if (OwnerComp.GetAIOwner() == nullptr) //|| OwnerComp.GetAIOwner()->GetPawn() == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller is nullptr"));
		return EBTNodeResult::Failed;
	}

	RandomLocation = FVector::ZeroVector;
	if (OwnerComp.GetOwner()->HasAuthority())
		RandomLocation = FindRandomLocation(OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation(), SearchRadius);
	else
		Server_FindRandomLocation(OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation(), SearchRadius);
	if (RandomLocation == FVector::ZeroVector)
		return EBTNodeResult::Failed;
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(TargetKey.SelectedKeyName, RandomLocation);
	return EBTNodeResult::Succeeded;
}

void UBTT_FindRandomLocation::Server_FindRandomLocation_Implementation(FVector Origin, float InSearchRadius)
{
	RandomLocation = FindRandomLocation(Origin, InSearchRadius);
}

FVector UBTT_FindRandomLocation::FindRandomLocation(FVector Origin, float InSearchRadius)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (NavSys)
	{
		FNavLocation OutRandomLocation;
		if (NavSys->GetRandomReachablePointInRadius(Origin, InSearchRadius, OutRandomLocation))
		{
			//OwnerComp.GetBlackboardComponent()->SetValueAsVector(TargetKey.SelectedKeyName, RandomLocation.Location);
			DrawDebugSphere(GetWorld(), OutRandomLocation.Location, 50.f, 4, FColor::Green, false, 1.0f);
			return OutRandomLocation.Location;
		}
	}
	return FVector::ZeroVector;
}

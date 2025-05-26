// Fill out your copyright notice in the Description page of Project Settings.

#include "BTS_CheckTargetInRange.h"
#include "../Controllers/ZAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_CheckTargetInRange::UBTS_CheckTargetInRange()
{
	NodeName = "CheckTargetInRange";
	ActiveRange = 100.f;
}

void UBTS_CheckTargetInRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AZAIController* ZController = Cast<AZAIController>(OwnerComp.GetAIOwner());
	if (ZController)
	{
		AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorKey.SelectedKeyName));
		if (TargetActor)
		{
			if (FVector::Dist(TargetActor->GetActorLocation(), ZController->GetPawn()->GetActorLocation()) < ActiveRange)
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), true);
			}
			else
				OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), false);
		}
	}
}

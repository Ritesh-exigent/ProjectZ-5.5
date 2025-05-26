// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_CheckTargetDetection.h"
#include "../Controllers/ZAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_CheckTargetDetection::UBTS_CheckTargetDetection()
{
	NodeName = "CheckTargetDetection";
}

void UBTS_CheckTargetDetection::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AZAIController* ZController = Cast<AZAIController>(OwnerComp.GetAIOwner());
	if (ZController)
	{
		bool bValue = ZController->IsTargetDetected();
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), bValue);
		if (bValue)
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetActorKey.SelectedKeyName, ZController->GetTargetActor());
	}
}

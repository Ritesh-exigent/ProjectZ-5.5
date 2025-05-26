// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Attack.h"
#include "../Controllers/ZAIController.h"

UBTT_Attack::UBTT_Attack()
{
	NodeName = "Attack";
}

EBTNodeResult::Type UBTT_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AZAIController* Controller = Cast<AZAIController>(OwnerComp.GetAIOwner());
	if (Controller)
	{
		Controller->Attack(bOverride);
		//UE_LOG(LogTemp, Warning, TEXT("Attacking"));
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_CheckTargetInRange.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API UBTS_CheckTargetInRange : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTS_CheckTargetInRange();

protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category="Values")
	float ActiveRange;

	UPROPERTY(EditAnywhere, Category="Values")
	FBlackboardKeySelector TargetActorKey;

};

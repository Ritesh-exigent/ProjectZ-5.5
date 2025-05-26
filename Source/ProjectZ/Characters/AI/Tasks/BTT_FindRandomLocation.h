// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_FindRandomLocation.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API UBTT_FindRandomLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTT_FindRandomLocation();

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	UFUNCTION(Server, Reliable)
	void Server_FindRandomLocation(FVector Origin, float InSearchRadius);
	void Server_FindRandomLocation_Implementation(FVector Origin, float InSearchRadius);
	FVector FindRandomLocation(FVector Origin, float InSearchRadius);

	//UPROPERTY(Replicated)
	FVector RandomLocation;
	
public:

	UPROPERTY(EditAnywhere, Category="Settings | Values")
	FBlackboardKeySelector TargetKey;
	UPROPERTY(EditAnywhere, Category="Settings | Values")
	float SearchRadius;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_ChasePlayer.generated.h"

class ASPlayer;
class AZAIController;

UCLASS()
class PROJECTZ_API UBTT_ChasePlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:

	UBTT_ChasePlayer();
	
protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaTime) override;


private:

	UPROPERTY(EditAnywhere, Category = "Settings | Values")
	float AcceptRadius;

	int32 CurrentPathIndex;
	FVector TargetLocation;
	FVector CurrentPathPoint;
	TArray<FVector> PathPoints;

	FVector PreviousLocation;

	AActor* TargetActor;
	ASPlayer* PlayerPawn;
	AZAIController* ZController;

	bool CalcPath();

};

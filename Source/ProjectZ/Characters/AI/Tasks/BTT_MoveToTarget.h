// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_MoveToTarget.generated.h"

class ASPlayer;
class AZGameMode;
class AZAIController;

UCLASS()
class PROJECTZ_API UBTT_MoveToTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTT_MoveToTarget();
	
protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaTime) override;
	
private:

	UPROPERTY(EditAnywhere, Category="Settings | Values")
	FBlackboardKeySelector TargetKey;
	UPROPERTY(EditAnywhere, Category="Settings | Values")
	float AcceptRadius;
	UPROPERTY(EditAnywhere, Category="Settings | Values")
	bool bChasePlayer;

	int32 CurrentPathIndex;
	FVector TargetLocation;
	FVector CurrentPathPoint;
	FVector PreviousLocation;
	TArray<FVector> PathPoints;

	bool bMoveFinish;
	EBTNodeResult::Type MoveResult;

	UPROPERTY(Replicated)
	ASPlayer* PlayerPawn;
	AZGameMode* ZGameMode;
	AZAIController* ZController;

	FVector SelectRandomPlayerLocation();
	FVector GetClosestPlayerLocation(UBehaviorTreeComponent& OwnerComp);
	double CheckDistance(UBehaviorTreeComponent& OwnerComp, int32 Index);

	UFUNCTION(Server, Reliable)
	void Server_InitMove();
	void Server_InitMove_Implementation();
	void InitMove();

	UFUNCTION(Server, Reliable)
	void Server_UpdateMove();
	void Server_UpdateMove_Implementation();
	void UpdateMove();
};

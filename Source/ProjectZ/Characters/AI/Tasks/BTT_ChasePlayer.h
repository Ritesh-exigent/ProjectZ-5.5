// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_ChasePlayer.generated.h"

class ASPlayer;
class AZAIController;

USTRUCT()
struct FChaseMemory {

	GENERATED_BODY()

public:

	FChaseMemory() {
		//HalfHeight = 96.f;
		PreviousLocation = FVector(0.f);
		TargetActor = nullptr;
		ZController = nullptr;
	}

	UPROPERTY()
	float HalfHeight;
	UPROPERTY()
	int32 CurrentPathIndex;
	UPROPERTY()
	FVector PreviousLocation;
	UPROPERTY()
	TArray<FVector> PathPoints;
	UPROPERTY()
	AActor* TargetActor;
	UPROPERTY()
	AZAIController* ZController;
};

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
	virtual uint16 GetInstanceMemorySize() const override;

private:

	UPROPERTY(EditAnywhere, Category = "Settings | Values")
	float AcceptRadius;

	float HalfHeight;
	int32 CurrentPathIndex;
	FVector TargetLocation;
	TArray<FVector> PathPoints;

	FVector PreviousLocation;

	AActor* TargetActor;
	ASPlayer* PlayerPawn;
	AZAIController* ZController;

	int Count;
	bool CalcPath(FChaseMemory* ChaseMemory);

};

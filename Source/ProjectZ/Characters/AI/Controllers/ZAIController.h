// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ProjectZ/Characters/MoveUtils.h"
#include "ZAIController.generated.h"

class AZEnemy;
class UBehaviorTree;
class UBlackboardComponent;
class UBehaviorTreeComponent;

UCLASS()
class PROJECTZ_API AZAIController : public AAIController
{
	GENERATED_BODY()

public:

	AZAIController();

protected:

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	
	/*UPROPERTY(EditDefaultsOnly, Category="Settings | BTree")
	UBehaviorTreeComponent* BTComp;
	UPROPERTY(EditDefaultsOnly, Category="Settings | BTree")
	UBlackboardComponent* BBComp;*/
	
	UPROPERTY(EditAnywhere, Category="Settings | Values")
	UBehaviorTree* BTree;

	UPROPERTY(BlueprintReadWrite);
	AZEnemy* Z_Pawn;
	UPROPERTY(BlueprintReadWrite);
	bool bIsTargetDetected;
	UPROPERTY(BlueprintReadWrite);
	AActor* TargetActor;
private:



public:

	void RequestAIMovement(FVector InTargetLocation, EMoveState InState, float InAcceptanceRadius);
	void AbortAIMovement();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAIMovementCompleted();
	void PauseMovement(bool InValue);

	__inline AActor* GetTargetActor() { return TargetActor; }
	__inline bool IsTargetDetected() { return bIsTargetDetected; }

	void Attack(bool bOverride = false);
	void StopBehaviorTree();

	UFUNCTION(BlueprintCallable)
	void OnTargetFound(AActor* InTargetActor);

};

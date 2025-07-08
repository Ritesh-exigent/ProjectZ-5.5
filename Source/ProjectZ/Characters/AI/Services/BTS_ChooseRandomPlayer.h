// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_ChooseRandomPlayer.generated.h"

class AZGameMode;

UCLASS()
class PROJECTZ_API UBTS_ChooseRandomPlayer : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTS_ChooseRandomPlayer();

protected:

	UPROPERTY(EditAnywhere, Category="Values")
	float RecheckCooldown;

	UPROPERTY(EditAnywhere, Category="Values")
	float CurrentTime;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;
private:

	AZGameMode* ZGameMode;
	
	void SelectRandomPlayer(UBehaviorTreeComponent& OwnerComp);
};

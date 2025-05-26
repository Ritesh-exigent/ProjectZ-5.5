// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectZ/Characters/AI/Perception/ZAISenseBase.h"
#include "ZAISense_Sight.generated.h"

class AZEnemy;
class ASPlayer;
class UPerceptionComponent;

USTRUCT()
struct FZAISightConfig {
	GENERATED_BODY()

public:

	FZAISightConfig() {
		FOV = 90.f;
		SightRange = 5000.f;
		LostSightRange = 6000.f;
	}

	UPROPERTY(EditAnywhere, Category="Config")
	float FOV;

	UPROPERTY(EditAnywhere, Category="Config")
	float SightRange;

	UPROPERTY(EditAnywhere, Category="Config")
	float LostSightRange;

};

UCLASS(Blueprintable, EditInlineNew)
class PROJECTZ_API UZAISense_Sight : public UZAISenseBase
{
	GENERATED_BODY()
	
public:

	UZAISense_Sight();

	virtual void InitSense(AActor* OwningActor, UPerceptionComponent* InComponent) override;
	virtual void ActivateSense() override;
	virtual void DeactivateSense() override;
	virtual void UpdateSense(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category="Settings | Values")
	FZAISightConfig SightConfig;

private:

	bool bIsAlert;
	
	AZEnemy* Enemy;
	TArray<AActor*> Players;

	void DetectPlayers();
	bool IsDetected(FVector PlayerLocation);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ZAISenseBase.generated.h"

class AZEnemy;
class UPerceptionComponent;

USTRUCT()
struct FZAISenseConfig {
	GENERATED_BODY()

public:

	FZAISenseConfig() {
		UpdateFrequency = 0.01f;
		Importance = 0;
		MaxAge = 5.f;

	}

	UPROPERTY(EditAnywhere, Category="Config")
	float UpdateFrequency;
	
	UPROPERTY(EditAnywhere, Category="Config")
	uint8 Importance;

	UPROPERTY(EditAnywhere, Category="Config")
	float MaxAge;

};

UCLASS(abstract, EditInlineNew)
class PROJECTZ_API UZAISenseBase : public UObject
{
	GENERATED_BODY()
	
public:

	UZAISenseBase();
	
	virtual void InitSense(AActor* OwningActor, UPerceptionComponent* InComponent);
	virtual void ActivateSense() {};
	virtual void DeactivateSense() {};
	virtual void UpdateSense(float DeltaTime) {};
	void TickSense(float DeltaTime);

	__inline FZAISenseConfig GetSenseConfig() { return SenseConfig; }
	__inline AActor* GetOwningEnemy() { return Owner; }
	__inline int32 GetSenseID() { return ID; }
	__inline UPerceptionComponent* GetPerceptionComponent() { return PComp; }

	UPROPERTY(EditAnywhere, Category = "Settings | Values")
	FZAISenseConfig SenseConfig;
private:

	int32 ID;
	float UpdateTime;
	AActor* Owner;
	UPerceptionComponent* PComp;

};

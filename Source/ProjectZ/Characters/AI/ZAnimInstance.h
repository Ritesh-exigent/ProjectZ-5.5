// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ZAnimInstance.generated.h"

class AZEnemy;

UCLASS()
class PROJECTZ_API UZAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UZAnimInstance();

protected:

	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	
public:

	UPROPERTY(BlueprintReadOnly)
	float Speed;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDead;

	void PlayMontage(UAnimMontage* InMontage, bool bOverride = false);

	void UpdateAnimState();

private:

	AZEnemy* Owner;


	void CastZOwner();
};

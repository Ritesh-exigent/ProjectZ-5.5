// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SPAnimInstance.generated.h"

class ASPlayer;

UCLASS()
class PROJECTZ_API USPAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	USPAnimInstance();

protected:

	void NativeBeginPlay() override;
	void NativeUpdateAnimation(float DeltaSeconds) override;

public:

	UPROPERTY(BlueprintReadOnly)
	float Speed;
	UPROPERTY(BlueprintReadOnly)
	float Direction;
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentWeaponType;
	UPROPERTY(BlueprintReadOnly)
	float LookX;
	
	void UpdateAnimMontage(UAnimMontage* InMontage);

private:
		
	ASPlayer* PlayerOwner;
	UAnimMontage* PlayerMontage;

	void CastPlayerPawn();
	void UpdateAnimState();
};

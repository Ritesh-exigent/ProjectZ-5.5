// Fill out your copyright notice in the Description page of Project Settings.


#include "ZAnimInstance.h"
#include "ZEnemy.h"
#include "../MoveComponent.h"

UZAnimInstance::UZAnimInstance()
{
	Owner = nullptr;
	Speed = 0.f;
	bIsDead = false;
}

void UZAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	CastZOwner();
}

void UZAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	UpdateAnimState();
}

void UZAnimInstance::PlayMontage(UAnimMontage* InMontage, bool bOverride)
{
	if (bOverride)
	{
		Montage_Stop(1.f);
		Montage_Play(InMontage);
	}
	else if (!IsAnyMontagePlaying())
	{
		UE_LOG(LogTemp, Warning, TEXT("Montage Playing"));
		Montage_Play(InMontage);
	}
}

void UZAnimInstance::UpdateAnimState()
{
	if (!Owner)
	{
		CastZOwner();
		return;
	}

	Speed = Owner->GetMoveComponent()->GetServerSpeed();
	//bIsDead = Owner->IsDead();
	//UE_LOG(LogTemp, Warning, TEXT("Anim IsDead: %d"), bIsDead);
}

void UZAnimInstance::CastZOwner()
{
	Owner = Cast<AZEnemy>(TryGetPawnOwner());
}

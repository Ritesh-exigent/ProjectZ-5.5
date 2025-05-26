// Fill out your copyright notice in the Description page of Project Settings.


#include "SPAnimInstance.h"
#include "SPlayer.h"
#include "../MoveComponent.h"
#include "ProjectZ/Weapons/WeaponComponent.h"
#include "Net/UnrealNetwork.h"

USPAnimInstance::USPAnimInstance()
{
	PlayerOwner = nullptr;
	Speed = 0.f;
	Direction = 0.f;
	
}

void USPAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	CastPlayerPawn();
}

void USPAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!PlayerOwner)
	{
		CastPlayerPawn();
		return;
	}

	UpdateAnimState();
	/*if (PlayerMontage)
	{
		if(!IsAnyMontagePlaying())
		Montage_Play(PlayerMontage);
	}*/
}

void USPAnimInstance::UpdateAnimMontage(UAnimMontage* InMontage)
{
	PlayerMontage = InMontage;
}

void USPAnimInstance::CastPlayerPawn()
{
	if (APawn* OwnerPawn = TryGetPawnOwner())
	{
		PlayerOwner = Cast<ASPlayer>(OwnerPawn);
	}
}

void USPAnimInstance::UpdateAnimState()
{
	if (!PlayerOwner || !PlayerOwner->GetMoveComponent() || !PlayerOwner->GetWeaponComponent())
		return;

	Speed = PlayerOwner->GetMoveComponent()->GetServerSpeed();
	Direction = PlayerOwner->GetMoveComponent()->GetMoveDirection();
	CurrentWeaponType = PlayerOwner->GetWeaponComponent()->GetCurrentWeaponType();
	LookX = PlayerOwner->GetLookYValue();
	//UE_LOG(LogTemp, Warning, TEXT("Player: %s, LookX: %f"), *GetOwningActor()->GetName(), LookX);
}

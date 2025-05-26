// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "HDComponent.h"
#include "MoveComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"



ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Collider = CREATE(UCapsuleComponent, "CapsuleComponent");
	SetRootComponent(Collider);
	SKMesh = CREATE(USkeletalMeshComponent, "SkeletalMeshComponent");
	SKMesh->SetupAttachment(Collider);

	MoveComp = CREATE(UMoveComponent, "MoveComponent");
	HDComp = CREATE(UHDComponent, "HDComponent");

	bReplicates = true;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
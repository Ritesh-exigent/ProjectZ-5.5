// Fill out your copyright notice in the Description page of Project Settings.


#include "ZEnemy.h"
#include "ZAnimInstance.h"
#include "../MoveComponent.h"
#include "ProjectZ/Weapons/Ammunition.h"
#include "ProjectZ/Characters/AI/Manager/ZEnemyManager.h"
#include "ProjectZ/Characters/AI/Perception/PerceptionComponent.h"

// Sets default values
AZEnemy::AZEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PerceptionComp = CREATE(UPerceptionComponent, "PerceptionComponent");

}

// Called when the game starts or when spawned
void AZEnemy::BeginPlay()
{
	Super::BeginPlay();
	ZController = GetWorld()->SpawnActor<AZAIController>(ControllerClass);//Cast<AZAIController>(GetController());
	if (ZController)
		ZController->Possess(this);

	AnimInst = Cast<UZAnimInstance>(SKMesh->GetAnimInstance());
	if (AnimInst)
	{
		AnimInst->OnMontageEnded.AddDynamic(this, &AZEnemy::OnMontageEnded);
	}
	if (MoveComp)
	{
		MoveComp->SetIsControllingAI(true);
	}
	if (HDComp)
		HDComp->OnDeath.BindUFunction(this, TEXT("OnDeath"));
	
	Reset();
}

// Called every frame
void AZEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*if (GetController())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s ->AIC : %s"), *GetName(), *GetController()->GetName());
	}*/
}

void AZEnemy::InitDestroy()
{
	if (Manager)
	{
		DropAmmunitions();
		Manager->OnDeath(this);
		StartDestroy();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Manger Null in enemy"));
	}
}

void AZEnemy::PlayAttackMontage(bool bOverride)
{
	if (AnimInst)
	{
		GetMoveComponent()->PauseMovement(true);
		//UE_LOG(LogTemp, Warning, TEXT("Enemy Attacking"));
		int32 RandomNum = FMath::RandRange(0, AttackMontages.Num() - 1);
		AnimInst->PlayMontage(AttackMontages[RandomNum], bOverride);
	}
}

void AZEnemy::OnMontageEnded(UAnimMontage* InMontage, bool bInterrupted)
{
	//UE_LOG(LogTemp, Warning, TEXT("Attack Ended"));
	if (InMontage)
		GetMoveComponent()->PauseMovement(false);
}

void AZEnemy::NetMulticast_PlayAttackMontage_Implementation(bool bOverride)
{
	PlayAttackMontage(bOverride);
}

void AZEnemy::DropAmmunitions()
{	
	UE_LOG(LogTemp, Warning, TEXT("Drop Ammo"));
	if (FMath::RandRange(0, 10) < 2)
		return;

	for (auto AmmoClass : Drops)
	{
		FActorSpawnParameters ASP;
		ASP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		FVector Location = GetActorLocation() + FVector(FMath::FRandRange(-50.f, 50.f), FMath::FRandRange(-50.f, 50.f), 0.f);
		AAmmunition* Ammo = GetWorld()->SpawnActor<AAmmunition>(AmmoClass, Location, FRotator::ZeroRotator, ASP);
	} 
}

void AZEnemy::Attack(bool bOverride)
{
	PlayAttackMontage(bOverride);
	if (HasAuthority())
		NetMulticast_PlayAttackMontage(bOverride);
}

void AZEnemy::OnDeath()
{
	if(!ZController)
		UE_LOG(LogTemp, Warning, TEXT("Controller Died!"));

	if (MoveComp && ZController)
	{	
		ZController->StopBehaviorTree();
		MoveComp->AbortAIMovement();
		InitDestroy();
		UE_LOG(LogTemp, Warning, TEXT("Enemy Died!"));
	}
}

void AZEnemy::Reset()
{
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);

	if(ZController)
	ZController->UnPossess();
}

void AZEnemy::Init()
{
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	SetActorEnableCollision(true);

	if (ZController)
		ZController->Possess(this);
}

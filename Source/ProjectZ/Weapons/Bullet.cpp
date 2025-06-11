// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Characters/HDComponent.h"
#include "./ProjectZ/Characters/Player/SPlayer.h"
#include "./ProjectZ/Characters/AI/ZEnemy.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBullet, Log, All);
DEFINE_LOG_CATEGORY(LogBullet);

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;
	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(Collider);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	Mesh->SetupAttachment(Collider);
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));


	bReplicates = true;
	SetReplicateMovement(true);
	bFire = false;
	Damage = 0;
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();
	Collider->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnBeginOverlap);
	
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority() && bFire)
	{
		AddActorWorldOffset(GetActorForwardVector() * Speed * DeltaTime);
		//DrawDebugSphere(GetWorld(), GetActorLocation(), 20.f, 6, FColor::Cyan, false, 5.f);
		//UE_LOG(LogTemp, Warning, TEXT("Bullet Location: %s"), *GetActorLocation().ToString());
		//UE_LOG(LogTemp, Warning, TEXT("Bullet Speed: %f"), Speed);
		Lifetime += DeltaTime;
		if (Lifetime >= MaxLifetime)
			Destroy();
	}
}

void ABullet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABullet, Speed);
	DOREPLIFETIME(ABullet, Lifetime);
	DOREPLIFETIME(ABullet, bFire);
}

void ABullet::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogBullet, Warning, TEXT("Bullet overlapped!"));
	if (HasAuthority())
	{
		//DrawDebugSphere(GetWorld(), GetActorLocation(), 40.f, 6, FColor::Blue, false, 10.f);
		UE_LOG(LogBullet, Warning, TEXT("Bullet Hit : %s"), *OtherActor->GetName());
		if (OtherActor->ActorHasTag(FName("Enemy")))
		{
			AZEnemy* Enemy = Cast<AZEnemy>(OtherActor);
			if(Enemy)
			Enemy->GetHDComponent()->TakeDamage(Damage);
		}
		Destroy(true);
	}
	else
	{
		UE_LOG(LogBullet, Warning, TEXT("bullet has no authority"));
	}
}

void ABullet::Server_StartSimulation_Implementation()
{
	bFire = true;
}


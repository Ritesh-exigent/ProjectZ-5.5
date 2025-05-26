// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class PROJECTZ_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ABullet();

protected:
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, Category="Settings | Defaults")
	class USphereComponent* Collider;
	UPROPERTY(EditDefaultsOnly, Category="Settings | Defaults")
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Settings | Values")
	class UNiagaraSystem* BulletFx;
	UPROPERTY(Replicated, EditAnywhere, Category = "Settings | Values")
	float Speed;
	UPROPERTY(EditAnywhere, Category = "Settings | Values")
	float MaxLifetime;

	float Damage;

private:

	UPROPERTY(Replicated)
	float Lifetime;

	UPROPERTY(Replicated)
	bool bFire;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void Server_StartSimulation_Implementation();

public:

	UFUNCTION(Server, Reliable)
	void Server_StartSimulation();

	__inline void SetDamage(float InAmount) { Damage = InAmount; }
	__inline void StartSimulation() { bFire = true; }
	
};

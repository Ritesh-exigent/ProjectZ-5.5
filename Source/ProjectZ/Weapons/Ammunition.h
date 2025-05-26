// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ammunition.generated.h"

class USphereComponent;
class UStaticmeshComponent;

UCLASS()
class PROJECTZ_API AAmmunition : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmmunition();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Settings | Defaults")
	USphereComponent* Collider;
	UPROPERTY(EditDefaultsOnly, Category = "Settings | Defaults")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, Category = "Settings | Values")
	int32 FixedAmount;
	UPROPERTY(EditDefaultsOnly, Category = "Settings | Values")
	int32 MinRandomAmount;
	UPROPERTY(EditDefaultsOnly, Category = "Settings | Values")
	int32 MaxRandomAmount;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Server, Reliable)
	void Server_InitDestroy();
	void Server_InitDestroy_Implementation();

public:

	void InitDestroy();

	int32 GetDropAmmoAmount(bool bGetFixedAmount);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HDComponent.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnDeath);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTZ_API UHDComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHDComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:

	UPROPERTY(ReplicatedUsing = OnRep_HealthUpdate)
	float C_Health;
	UPROPERTY(EditAnywhere, Category="Settings | Values")
	float MaxHealth;

	UFUNCTION(Server, Reliable)
	void Server_UpdateHealth(float InHealth);
	void Server_UpdateHealth_Implementation(float InHealth);

	UFUNCTION(Server, Reliable)
	void Server_TakeDamage(float InAmount);
	void Server_TakeDamage_Implementation(float InAmount);

	UFUNCTION()
	void OnRep_HealthUpdate();
	
	void Damage(float InAmount);

public:

	FOnDeath OnDeath;

	void TakeDamage(float InAmount);
	void AddHealth(float InAmount);

	__inline float GetHealth() { return C_Health; }
	__inline float GetMaxHealth() { return MaxHealth; }
	__inline bool IsDead() { return (C_Health <= 0) ? true : false; }
};

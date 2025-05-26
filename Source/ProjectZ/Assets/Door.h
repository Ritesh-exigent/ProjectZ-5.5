// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Quest/QuestItem.h"
#include "ProjectZ/Interfaces/InteractionInterface.h"
#include "Door.generated.h"

class ASPlayer;
class UBoxComponent;
class UAnimationAsset;
class USkeletalMeshComponent;

UCLASS()
class PROJECTZ_API ADoor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	UPROPERTY(EditDefaultsOnly, Category="Settings | Components")
	UBoxComponent* Collider;

	UPROPERTY(EditDefaultsOnly, Category="Settings | Components")
	USkeletalMeshComponent* SKMesh;

	UPROPERTY(EditDefaultsOnly, Category="Settings | Values", meta=(AllowPrivateAccess = true))
	UAnimationAsset* OpenAnimation;
	UPROPERTY(EditDefaultsOnly, Category="Settings | Values", meta=(AllowPrivateAccess = true))
	UAnimationAsset* CloseAnimation;

	UPROPERTY(ReplicatedUsing = OnRep_ToggleDoor)
	bool bOpenDoor;

	void ToggleDoor();
	UFUNCTION()
	void OnRep_ToggleDoor();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_SetDoorCollision(ECollisionEnabled::Type InType);
	void NetMulticast_SetDoorCollision_Implementation(ECollisionEnabled::Type InType);

public:

	void Interact(ASPlayer* InPlayer) override;
	
};

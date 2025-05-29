// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestUtils.h"
#include "../Interfaces/InteractionInterface.h"
#include "QuestItem.generated.h"

class AZGameState;


UCLASS()
class PROJECTZ_API AQuestItem : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQuestItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, Category="Values")
	FName QuestID;
	UPROPERTY(EditAnywhere, Category="Values")
	int32 SubQuestID;
	UPROPERTY(EditDefaultsOnly, Category="Values")
	TEnumAsByte<EQuestItemType> ItemType;

	UPROPERTY(Replicated)
	bool bIsActive;
	AZGameState* ZGameState;

	UFUNCTION(Server, Reliable)
	void Server_SetItemActive(bool bValue);
	void Server_SetItemActive_Implementation(bool bValue);

public:

	__inline bool IsOfType(EQuestItemType InType) { return ItemType == InType; }

	void Activate();
	void Deactivate();
	void Interact(ASPlayer* InPlayer) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestUtils.h"
#include "../Interfaces/InteractionInterface.h"

#include "QuestItem.generated.h"

class AZGameState;
class UWidgetComponent;

UCLASS()
class PROJECTZ_API AQuestItem : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQuestItem();

protected:

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnInteract();

	UPROPERTY(EditAnywhere, Category="Values")
	FName QuestID;
	UPROPERTY(EditAnywhere, Category="Values")
	int32 SubQuestID;
	UPROPERTY(EditAnywhere, Category="Values")
	bool bCanSpawnEnemies;
	UPROPERTY(EditDefaultsOnly, Category="Values")
	TEnumAsByte<EQuestItemType> ItemType;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category="Values")
	UStaticMeshComponent* Mesh;
	UPROPERTY(EditDefaultsOnly, Category="Values")
	UWidgetComponent* MarkerWidgetComponent;

	UPROPERTY(ReplicatedUsing = OnRep_IsInteracted)
	bool bIsInteracted;
	UPROPERTY(ReplicatedUsing = OnRep_IsQuestActive)
	bool bIsActive;
	AZGameState* ZGameState;
	
	UFUNCTION()
	void OnRep_IsInteracted();
	UFUNCTION()
	void OnRep_IsQuestActive();

	UFUNCTION(Client, Reliable)
	void Client_OnInteract();
	void Client_OnInteract_Implementation();

public:

	__inline bool IsOfType(EQuestItemType InType) { return ItemType == InType; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	inline bool IsItemActive() { return bIsActive; }
	void Activate();
	void Deactivate();
	void Interact(ASPlayer* InPlayer) override;
};

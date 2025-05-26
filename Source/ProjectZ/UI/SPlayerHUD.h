// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Quest/QuestManager.h"
#include "SPlayerHUD.generated.h"

class AWeapon;
class USizeBox;
class UTextBlock;
class AZGameState;
class UProgressBar;
class UVerticalBox;
UCLASS()
class PROJECTZ_API USPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

protected:

	bool Initialize() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


private:

	AZGameState* ZGameState;

	void InitGameState();
	
	UFUNCTION()
	void OnEnemyWaveInfoUpdated(FEnemyWaveInfo NewWaveInfo);

	UFUNCTION()
	void OnQuestUpdated(FText InQuestTitle);
	UFUNCTION()
	void OnSubQuestUpdated(FSubQuestInfo InQuestInfo);

public:

	UPROPERTY(Replicated, meta = (BindWidget))
	UTextBlock* T_RoundNumber;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_CurrentAmmo;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_TotalAmmo;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* PB_Health;

	UPROPERTY(meta = (BindWidget))
	USizeBox* SB_QuestContainer;
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_QuestContainer;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_QuestTitle;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_SubQuestTitle;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_Count;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_Time;

	float MaxHealth;

	void AddQuestDescription(FQuestDescription InQuestDescription);
	
	void SetCurrentAmmo(int32 Amount);
	void SetTotalAmmo(int32 Amount);
	void SetCurrentHealth(float InCurrentHealth);
	void SetMaxHealth(float InMaxHealth);


};

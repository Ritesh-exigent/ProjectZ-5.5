// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerHUD.h"
#include "Net/UnrealNetwork.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "./ProjectZ/GameStates/ZGameState.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

bool USPlayerHUD::Initialize()
{
	bool bSuccess = Super::Initialize();

	if (!T_RoundNumber || !T_CurrentAmmo || !T_TotalAmmo || !PB_Health)
		return false;
	//if(GetOwningPlayerPawn()->GetLocalRole() == ROLE_AutonomousProxy)
	//Server_InitGameState();
	MaxHealth = 200.f;
	InitGameState();
	return bSuccess;

	
}

void USPlayerHUD::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USPlayerHUD, T_RoundNumber);
	//DOREPLIFETIME(USPlayerHUD, T_QuestTitle);
	//DOREPLIFETIME(USPlayerHUD, T_SubQuestTitle);
}

void USPlayerHUD::OnEnemyWaveInfoUpdated(FEnemyWaveInfo NewWaveInfo)
{
	T_RoundNumber->SetText(FText::FromString(FString::FromInt(NewWaveInfo.WaveNum)));
}

void USPlayerHUD::OnQuestUpdated(FText InQuestTitle)
{
	//UE_LOG(LogTemp, Warning, TEXT("Quest Title Updated"));
	T_QuestTitle->SetText(InQuestTitle);
}

void USPlayerHUD::OnSubQuestUpdated(FSubQuestInfo InSubQuestInfo)
{
	T_SubQuestTitle->SetText(InSubQuestInfo.SubQuestTitle);
	if (InSubQuestInfo.SubQuestType == EQuestType::QT_Collect)
	{
		T_Count->SetText(FText::FromString(FString::Printf(TEXT("0/%d"), InSubQuestInfo.TotalCount)));
	}
	else if (InSubQuestInfo.SubQuestType == EQuestType::QT_Time)
		T_Time->SetText(FText::FromString(FString::Printf(TEXT("0/%d"), InSubQuestInfo.TotalTime)));
}

void USPlayerHUD::InitGameState()
{
	if (GetWorld())
	{
		ZGameState = GetWorld()->GetGameState<AZGameState>();
		if (ZGameState)
		{
			ZGameState->OnEnemyWaveUpdated.BindUFunction(this, "OnEnemyWaveInfoUpdated");
			ZGameState->OnQuestUpdated.BindUFunction(this, "OnQuestUpdated");
			ZGameState->OnSubQuestUpdated.BindUFunction(this, "OnSubQuestUpdated");

			UE_LOG(LogTemp, Warning, TEXT("Gamestate is valid in splayerhud.cpp"));
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("Gamestate is nullptr in splayerhud.cpp"));
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("world is nullptr in splayerhud.cpp"));
	

}

void USPlayerHUD::AddQuestDescription(FQuestDescription InQuestDescription)
{
	UE_LOG(LogTemp, Warning, TEXT("QTitle: %s"), *InQuestDescription.QTitle.ToString());
	T_QuestTitle->SetText(InQuestDescription.QTitle);
	T_SubQuestTitle->SetText(InQuestDescription.SQTitle);
}

void USPlayerHUD::SetCurrentAmmo(int32 Amount)
{
	if(T_CurrentAmmo)
		T_CurrentAmmo->SetText(FText::FromString(FString::FromInt(Amount)));
}

void USPlayerHUD::SetTotalAmmo(int32 Amount)
{
	if(T_TotalAmmo)
		T_TotalAmmo->SetText(FText::FromString(FString::FromInt(Amount)));
}

void USPlayerHUD::SetCurrentHealth(float InCurrentHealth)
{
	if (PB_Health)
		PB_Health->SetPercent(InCurrentHealth / MaxHealth);
}

void USPlayerHUD::SetMaxHealth(float InMaxHealth)
{
	MaxHealth = InMaxHealth;
}
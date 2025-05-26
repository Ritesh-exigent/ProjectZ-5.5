// Fill out your copyright notice in the Description page of Project Settings.


#include "ZAISenseBase.h"

UZAISenseBase::UZAISenseBase()
{
	Owner = nullptr;
	ID = -1;
}

void UZAISenseBase::InitSense(AActor* OwningActor, UPerceptionComponent* InComponent)
{
	Owner = OwningActor;
	ID = OwningActor->GetUniqueID();
	PComp = InComponent;
}

void UZAISenseBase::TickSense(float DeltaTime) 
{
	if (!Owner || !PComp)
	{	
		UE_LOG(LogTemp, Warning, TEXT("Sense not init"));
		return;
	}

	UpdateTime += DeltaTime;
	if (UpdateTime >= SenseConfig.UpdateFrequency)
	{
		UpdateSense(DeltaTime);
		UpdateTime = 0.f;
	}

}

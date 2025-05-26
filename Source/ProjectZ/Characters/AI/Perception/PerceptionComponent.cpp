// Fill out your copyright notice in the Description page of Project Settings.


#include "PerceptionComponent.h"
#include "ZAISenseBase.h"


// Sets default values for this component's properties
UPerceptionComponent::UPerceptionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UPerceptionComponent::BeginPlay()
{
	Super::BeginPlay();

	//may need to use async for optimization
	if (Senses_C.Num() > 0)
	{
		Senses.SetNum(Senses_C.Num());
		for (int32 i=0; i<Senses_C.Num(); ++i)
		{
			Senses[i] = NewObject<UZAISenseBase>(this, Senses_C[i].Get());
			if (Senses[i])
			{
				Senses[i]->InitSense(GetOwner(), this);
				Senses[i]->ActivateSense();
				//Senses.Add(Sense);
			}
		}
	}
	
}


// Called every frame
void UPerceptionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Senses.Num() > 0)
	{
		for (int i=0; i<Senses.Num(); ++i)
		{
			if(Senses[i])
			Senses[i]->TickSense(DeltaTime);
		}
	}
}

void UPerceptionComponent::SetTargetActor(AActor* InTarget)
{
	TargetActor = InTarget;
	OnTargetFound.Broadcast(InTarget);
}


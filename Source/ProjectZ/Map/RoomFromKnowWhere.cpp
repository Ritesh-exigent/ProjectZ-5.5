// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomFromKnowWhere.h"

// Sets default values
ARoomFromKnowWhere::ARoomFromKnowWhere()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void ARoomFromKnowWhere::BeginPlay()
{
	Super::BeginPlay();
	//if (bShouldStayHidden) {
	//	SetActorHiddenInGame(true);
	//	SetActorEnableCollision(false);
	//}

	Super::BeginPlay();
	StartLocation = GetActorLocation();
}

// Called every frame
void ARoomFromKnowWhere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//float Time = GetWorld()->GetTimeSeconds();
	//FVector NewLocation = StartLocation;
	//NewLocation.Z += FMath::Sin(Time * FloatSpeed) * FloatAmplitude;
	//SetActorLocation(NewLocation);
	if (bShouldStayHidden) {
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}
	else
	{
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
	}
}


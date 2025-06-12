// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightManger.generated.h"


class ARoomFromKnowWhere;
UCLASS()
class PROJECTZ_API ALightManger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALightManger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<AActor*> FoundLights;

	void LightsToggleAll();
	void ToggleLight();

	UFUNCTION(BlueprintCallable)
	void MainRoomInit();

	void MainRoomFinalize();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UMaterialInterface* LightFunction;

	ARoomFromKnowWhere* RedRoom;
};

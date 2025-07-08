// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightManager.generated.h"

UCLASS()
class PROJECTZ_API ALightManager : public AActor
{
	GENERATED_BODY()
	
public:	

	ALightManager();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:


	
	
};

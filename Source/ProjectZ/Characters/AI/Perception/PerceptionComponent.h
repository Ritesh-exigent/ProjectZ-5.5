// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PerceptionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetFound, AActor*, TargetActor);

class UZAISenseBase;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTZ_API UPerceptionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPerceptionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category="Settings | Values")
	TArray<TSubclassOf<UZAISenseBase>> Senses_C;
	//TArray<TObjectPtr<UZAISenseBase>> Senses_C;

private:

	UPROPERTY()
	TArray<UZAISenseBase*> Senses;
	AActor* TargetActor;

public:

	UFUNCTION(BlueprintCallable)
	void SetTargetActor(AActor* InTarget);
	UPROPERTY(BlueprintAssignable)
	FOnTargetFound OnTargetFound;
};

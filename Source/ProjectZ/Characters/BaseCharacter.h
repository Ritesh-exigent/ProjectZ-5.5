// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ProjectZ/Characters/HDComponent.h"
#include "BaseCharacter.generated.h"

#define CREATE(Class, Name) CreateDefaultSubobject<Class>(FName(Name))

class UMoveComponent;
class UCapsuleComponent;
class USkeletalMeshComponent;

UCLASS()
class PROJECTZ_API ABaseCharacter : public APawn
{
	GENERATED_BODY()

public:
	
	ABaseCharacter();

protected:

	UPROPERTY(EditDefaultsOnly, Category="Settings | Defaults")
	UCapsuleComponent* Collider;
	UPROPERTY(EditAnywhere, Category="Settings | Defaults")
	USkeletalMeshComponent* SKMesh;
	UPROPERTY(EditDefaultsOnly, Category="Settings | Defaults")
	UMoveComponent* MoveComp;
	UPROPERTY(EditDefaultsOnly, Category="Settings | Defaults")
	UHDComponent* HDComp;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:

	//CapsuleComponent
	__inline UCapsuleComponent* GetCollider() { return Collider; }
	//SkeletalMeshComponent
	__inline USkeletalMeshComponent* GetMesh() { return SKMesh; }
	//MoveComponent
	__inline UMoveComponent* GetMoveComponent() { return MoveComp; }
	//HDComponent
	__inline UHDComponent* GetHDComponent() { return HDComp; }

	__inline bool IsDead() { return HDComp->IsDead(); }
};

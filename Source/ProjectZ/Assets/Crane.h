// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "EnhancedInput/Public/InputAction.h"
#include "../Interfaces/ZPawnInterface.h"
#include "Crane.generated.h"

class UCameraComponent;
class UPrimitiveComponent;
class UStaticMeshComponent;
class UInputMappingContext;
class UPhysicsConstraintComponent;

UCLASS()
class PROJECTZ_API ACrane : public APawn, public IZPawnInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACrane();

protected:
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* InController) override;
	virtual void UnPossessed() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Restart() override;

	UPROPERTY(EditAnywhere, Category="Settings | Components")
	UStaticMeshComponent* Root;
	UPROPERTY(EditAnywhere, Category="Settings | Components")
	UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, Category="Settings | Values")
	UInputMappingContext* IMC_Crane;
	UPROPERTY(EditAnywhere, Category="Settings | Values")
	UInputAction* CMoveAction;
	UPROPERTY(EditAnywhere, Category="Settings | Values")
	UInputAction* CDropAction;
	UPROPERTY(EditAnywhere, Category="Settings | Values")
	UInputAction* CAttachAction;
	UPROPERTY(EditAnywhere, Category="Settings | Values")
	UInputAction* CExitAction;
	
	UPROPERTY(Replicated)
	bool bStartMagnet;
	UPROPERTY(Replicated)
	bool bIsInUse;
	UPROPERTY(Replicated)
	bool bOverload;
	UPROPERTY(EditAnywhere, Category = "Settings | Values")
	UPhysicsConstraintComponent* PhysicsConstraint;
	UPROPERTY(EditAnywhere, Category = "Settings | Values")
	UStaticMeshComponent* Magnet;
	UPROPERTY(EditAnywhere, Category = "Settings | Values")
	UStaticMeshComponent* ControlComponent;
	UPROPERTY(EditAnywhere, Category = "Settings | Values")
	float Speed;
	UPROPERTY(EditAnywhere, Category = "Settings | Values")
	float MaxLength;
	UPROPERTY(EditAnywhere, Category = "Settings | Values", meta = (MakeEditWidget = true))
	FVector OverloadLocation;

	bool bIsLifting;
	bool bIsMoving;
	float Direction;
	float CurrentLength;
	//not relative actually
	FVector RelativeMagnetLocation;
	AActor* AttachedActor;

private:

	void PerformMove(const FInputActionValue& InValue);
	void FinishMove(const FInputActionValue& InValue);
	void StartMagnet(const FInputActionValue& InValue);
	void Detach(const FInputActionValue& InValue);
	void Exit(const FInputActionValue& InValue);

	void Overload(float DeltaTime);
	void Attach();

	UFUNCTION(Server, Reliable)
	void Server_SetInUse(bool bInValue);
	void Server_SetInUse_Implementation(bool bInValue);

	UFUNCTION(Server, Reliable)
	void Server_Exit();
	void Server_Exit_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_Lift(FVector MagnetLocation);
	void Server_Lift_Implementation(FVector MagnetLocation);

	UFUNCTION(Server, Reliable)
	void Server_DetachActor();
	void Server_DetachActor_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_AttachActor(AActor* Actor);
	void Server_AttachActor_Implementation(AActor* Actor);

	UFUNCTION(Server, Reliable)
	void Server_Move(FVector NewLocation);
	void Server_Move_Implementation(FVector NewLocation);

public:

	virtual void SetupPawnInputMappingContext(APlayerController* PC) override;
	UFUNCTION(BlueprintCallable)
	void SetControlMesh(UStaticMeshComponent* InComponent, UStaticMeshComponent* InMagnet, UPhysicsConstraintComponent* InPCComponent);
	__inline void SetOverload(bool bInValue) { bOverload = bInValue; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	inline bool IsOverloaded() { return bOverload; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	inline bool IsInUse() { return bIsInUse; }
};

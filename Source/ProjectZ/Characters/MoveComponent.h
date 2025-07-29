// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MoveUtils.h"
#include "MoveComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAIMoveComplete);

USTRUCT()
struct FMoveData {

	GENERATED_BODY()

public:

	FMoveData(){}

	FMoveData(int32 InSpeed, FVector InNetMovement, FVector InDirection)
	{
		NetMovement = InNetMovement;
		Speed = InSpeed;
		Direction = InDirection;
	}
	
	UPROPERTY()
	float Speed;
	UPROPERTY()
	FVector NetMovement;
	UPROPERTY()
	FVector Direction;

	void Print() {
		UE_LOG(LogTemp, Warning, TEXT("NetMovement: %s \n Speed: %f"), *NetMovement.ToString(), Speed);
	}
};

//USTRUCT()
//struct FServerMoveData {
//
//	GENERATED_BODY()
//
//public:
//
//	FServerMoveData(FVector	InReplicatedMovement)
//	{
//		ReplicatedMovement = InReplicatedMovement;
//	}
//	FVector ReplicatedMovement;
//};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTZ_API UMoveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMoveComponent();

protected:

	UPROPERTY(EditDefaultsOnly, Category="Settings | Values")
	float MaxServerSpeed;

	UPROPERTY(EditDefaultsOnly, Category="Settings | Values")
	float MaxRunSpeed;
	
	UPROPERTY(EditDefaultsOnly, Category="Settings | Values")
	float MaxWalkSpeed;

	UPROPERTY(EditAnywhere, Category="Settings | Values")
	float Acceleration;

	UPROPERTY(EditAnywhere, Category="Settings | Values")
	float Deceleration;

	UPROPERTY(EditAnywhere, Category="Settings | Values")
	float JumpZSpeed;

	UPROPERTY(EditAnywhere, Category="Settings | Values")
	bool bOrientRotationToMovement;
	
	UPROPERTY(Replicated)
	float SpeedOnServer;
	UPROPERTY(Replicated)
	float DirectionOnServer;
	UPROPERTY(ReplicatedUsing = OnRep_LocationOnServer)
	FVector LocationOnServer;


	float Speed;
	bool bPauseMovment;
	FVector CurrentDirection;
	FVector NetDirection;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FMoveData InData);
	void Server_SendMove_Implementation(FMoveData InData);
	bool Server_SendMove_Validate(FMoveData InData);

	UFUNCTION(Server, Reliable)
	void Server_AddMovement(FVector Direction, EMoveState InState);
	void Server_AddMovement_Implementation(FVector Direction, EMoveState InState);

	UFUNCTION(Server, Reliable)
	void Server_HandleOverallMovement(float DeltaTime);
	void Server_HandleOverallMovement_Implementation(float DeltaTime);

	UFUNCTION(Client, Reliable)
	void Client_UpdateMovement(FVector ServerLocation);
	void Client_UpdateMovement_Implementation(FVector ServerLocation);

	UFUNCTION()
	void OnRep_LocationOnServer();

private:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	bool bIsOwnerAI;
	bool bCanAIMove;
	bool bPauseMovement;
	float AcceptanceRadius;
	FVector AIDirection;
	FVector TargetLocation;
	EMoveState MoveState;
	EMoveResult MoveResult;
	
	void InterpDirection(float DeltaTime);
	void ApplyMovement(float DeltaTime);
	void ResetDirection(float DeltaTime);
	void CheckAIMoveResult();

	void CorrectMovement(float DeltaTime);
	float CalculateDirectionAngle(FVector InDirection);


public:

	void AddMovement(FVector Direction, EMoveState InState);
	void HandleOverallMovement(float DeltaTime);
	float GetMoveDirection() { return DirectionOnServer; }

	void RequestAIMovement(FVector InTargetLocation, EMoveState InState, float InAcceptanceRadius);
	void AbortAIMovement();
	void PauseMovement(bool InValue);

	void UpdateMovement(float DeltaTime);

	__inline void SetIsControllingAI(bool InValue) { bIsOwnerAI = InValue; }
	__inline bool IsAIMovementCompleted() { return MoveResult == EMoveResult::Completed; }
	__inline void SetMoveState(EMoveState InState) { MoveState = InState; }
	__inline float GetSpeed() { return Speed; }
	__inline float GetServerSpeed() { return SpeedOnServer; }

	FOnAIMoveComplete OnAIMoveComplete;
};

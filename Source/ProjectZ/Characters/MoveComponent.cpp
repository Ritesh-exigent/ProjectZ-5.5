// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveComponent.h"
#include "BaseCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UMoveComponent::UMoveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);

	bIsOwnerAI = false;
	bCanAIMove = false;
	MoveState = EMoveState::Idle;
	bOrientRotationToMovement = true;
}

 
// Called when the game starts
void UMoveComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentDirection = FVector::ZeroVector;
	NetDirection = FVector::ZeroVector;
	//must be set from editor
	/*MaxRunSpeed = 600.f;
	MaxWalkSpeed = 600.f;*/

	bPauseMovement = false;
}


// Called every frame
void UMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateMovement(DeltaTime);
}

void UMoveComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMoveComponent, SpeedOnServer);
	DOREPLIFETIME(UMoveComponent, DirectionOnServer);
	DOREPLIFETIME(UMoveComponent, LocationOnServer);
}

void UMoveComponent::Server_SendMove_Implementation(FVector InLocation, float InSpeed, FVector InDirection)
{
	GetOwner()->SetActorLocation(InLocation);
	SpeedOnServer = InSpeed;
	//UE_LOG(LogTemp, Warning, TEXT("Owner: %s, SpeedOnServer: %f"), *GetOwner()->GetName(), SpeedOnServer);
	if (InDirection.IsNearlyZero())
	{
		DirectionOnServer = 0.f;
	}
	else
	{
		/*float Dot = InDirection.Dot(GetOwner()->GetActorForwardVector());
		FVector Cross = InDirection.Cross(GetOwner()->GetActorForwardVector());
		float Direction = Cross.Dot(GetOwner()->GetActorUpVector());
		if (Direction < 0)
			Direction = -1.f;
		else
			Direction = 1.f;

		FVector Start = GetOwner()->GetActorLocation();
		DrawDebugLine(GetWorld(), Start, Start + GetOwner()->GetActorForwardVector() * 500.f, FColor::Red, false, 0.01f);
		DrawDebugLine(GetWorld(), Start, Start + InDirection * 500.f, FColor::Blue, false, 0.01f);
		float Angle = FMath::RadiansToDegrees(FMath::Atan2(Cross.Length(), Dot));*/
		DirectionOnServer = CalculateDirectionAngle(InDirection);//Angle * -Direction;
		//UE_LOG(LogTemp, Warning, TEXT("Angle: %f"), DirectionOnServer);
	}
}

bool UMoveComponent::Server_SendMove_Validate(FVector InLocation, float InSpeed, FVector InDirection)
{
	return true;
}

void UMoveComponent::Server_AddMovement_Implementation(FVector Direction, EMoveState InState)
{
	AddMovement(Direction, InState);
}

void UMoveComponent::Server_HandleOverallMovement_Implementation(float DeltaTime)
{
	HandleOverallMovement(DeltaTime);
}

void UMoveComponent::Client_UpdateMovement_Implementation(FVector Serverlocation)
{
	GetOwner()->SetActorLocation(Serverlocation);
}

void UMoveComponent::OnRep_LocationOnServer()
{
	GetOwner()->SetActorLocation(LocationOnServer);
}

void UMoveComponent::InterpDirection(float DeltaTime)
{	
	CurrentDirection = FMath::VInterpTo(CurrentDirection, NetDirection, DeltaTime, Acceleration);
	if (CurrentDirection.IsNearlyZero())
	{
		Speed = 0.f;
	}
	
}

void UMoveComponent::ApplyMovement(float DeltaTime)
{
	FVector NetMove;
	FHitResult MoveHit;
	NetMove = CurrentDirection * Speed * DeltaTime;
	//DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() + NetMove * 1000.f, FColor::Cyan, false, 0.1f);
	GetOwner()->AddActorWorldOffset(NetMove, true, &MoveHit);

	if(bOrientRotationToMovement)
	{
		FQuat NetQuat = NetMove.ToOrientationQuat();
		NetQuat.X = 0;
		NetQuat.Y = 0;
		GetOwner()->SetActorRotation(NetQuat);
	}

	if (MoveHit.bBlockingHit)
	{
		FHitResult ProjectionHit;
		FVector ProjectionDirection = FVector::VectorPlaneProject(CurrentDirection, MoveHit.Normal);
		NetMove = ProjectionDirection * Speed * DeltaTime;
		GetOwner()->AddActorWorldOffset(NetMove, true, &ProjectionHit);
	}

	SpeedOnServer = Speed;
	DirectionOnServer = CalculateDirectionAngle(CurrentDirection);
	LocationOnServer = GetOwner()->GetActorLocation();

	if (!bIsOwnerAI)
		OnRep_LocationOnServer();

}

void UMoveComponent::ResetDirection(float DeltaTime)
{
	NetDirection = FMath::VInterpTo(NetDirection, FVector::ZeroVector, DeltaTime, Deceleration);
}

void UMoveComponent::HandleOverallMovement(float DeltaTime)
{
	if (bPauseMovement) return;

	InterpDirection(DeltaTime);
	ApplyMovement(DeltaTime);
	ResetDirection(DeltaTime);
}

void UMoveComponent::AddMovement(FVector Direction, EMoveState InState)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		SetMoveState(InState);
		NetDirection += Direction;
		if (InState == EMoveState::Run)
			Speed = MaxRunSpeed;
		else
			Speed = MaxWalkSpeed;
		//Speed = 600.f;
		//UE_LOG(LogTemp, Warning, TEXT("On Server %s: Speed: %f, MRS: %f, MWS: %f"), *GetOwner()->GetName(), Speed, MaxRunSpeed, MaxWalkSpeed);
		if (NetDirection.Size() > 1.f)
			NetDirection.Normalize();
	}
	else
		Server_AddMovement(Direction, InState);

}

void UMoveComponent::RequestAIMovement(FVector InTargetLocation, EMoveState InState, float InAcceptanceRadius)
{
	//UE_LOG(LogTemp, Warning, TEXT("TragetLocation: %s"), *InTargetLocation.ToString());
	bCanAIMove = true;
	AcceptanceRadius = InAcceptanceRadius;
	AIDirection = (InTargetLocation - GetOwner()->GetActorLocation()).GetSafeNormal();
	AIDirection.Z = 0;
	//UE_LOG(LogTemp, Warning, TEXT("AiDirection: %s"), *AIDirection.ToString());
	TargetLocation = InTargetLocation;
	MoveState = InState;
	MoveResult = EMoveResult::InProgress;
}

void UMoveComponent::AbortAIMovement()
{
	bCanAIMove = false;
	AcceptanceRadius = 0.f;
	AIDirection = FVector::ZeroVector;
	TargetLocation = FVector::ZeroVector;
	MoveState = EMoveState::Idle;
	MoveResult = EMoveResult::Failed;
	Speed = 0.f;
	SpeedOnServer = 0.f;
}

void UMoveComponent::CheckAIMoveResult()
{
	float Distance = 9999999.f;
	Distance = FVector::Dist(TargetLocation, GetOwner()->GetActorLocation());
	if (Distance <= AcceptanceRadius)
	{
		bCanAIMove = false;
		AIDirection = FVector::ZeroVector;
		MoveResult = EMoveResult::Completed;
		SpeedOnServer = 0.f;
		Speed = 0.f;
	}
}

float UMoveComponent::CalculateDirectionAngle(FVector InDirection)
{
	float Dot = InDirection.Dot(GetOwner()->GetActorForwardVector());
	FVector Cross = InDirection.Cross(GetOwner()->GetActorForwardVector());
	float Direction = Cross.Dot(GetOwner()->GetActorUpVector());
	if (Direction < 0)
		Direction = -1.f;
	else
		Direction = 1.f;

	FVector Start = GetOwner()->GetActorLocation();
	DrawDebugLine(GetWorld(), Start, Start + GetOwner()->GetActorForwardVector() * 500.f, FColor::Red, false, 0.01f);
	DrawDebugLine(GetWorld(), Start, Start + InDirection * 500.f, FColor::Blue, false, 0.01f);
	float Angle = FMath::RadiansToDegrees(FMath::Atan2(Cross.Length(), Dot));
	return Angle * -Direction;
}

void UMoveComponent::PauseMovement(bool InValue)
{
	bPauseMovement = InValue;
}

void UMoveComponent::UpdateMovement(float DeltaTime)
{
	if (bIsOwnerAI)
	{
		if (bCanAIMove)
		{
			AIDirection = (TargetLocation - GetOwner()->GetActorLocation()).GetSafeNormal();
			AIDirection.Z = 0.f;
			AddMovement(AIDirection, MoveState);
			HandleOverallMovement(DeltaTime);
			CheckAIMoveResult();
			//UE_LOG(LogTemp, Warning, TEXT("%s, AI Move Update Called!"), *GetOwner()->GetName());
		}
		//UE_LOG(LogTemp, Warning, TEXT("%s, AI Update Called!"), *GetOwner()->GetName());
	}
	else
	{
		if (GetOwnerRole() == ROLE_Authority)
		{
			HandleOverallMovement(DeltaTime);
		}
		else if(GetOwnerRole() == ROLE_AutonomousProxy)
			Server_HandleOverallMovement(DeltaTime);

		//UE_LOG(LogTemp, Warning, TEXT("%s, Player Update Called"), *GetOwner()->GetName());
	}
}



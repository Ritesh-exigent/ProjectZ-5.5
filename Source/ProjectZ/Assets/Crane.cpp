// Fill out your copyright notice in the Description page of Project Settings.


#include "Crane.h"
#include "Net/UnrealNetwork.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Characters/Player/ZPlayerController.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

// Sets default values
ACrane::ACrane()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	Root = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshRoot"));
	SetRootComponent(Root);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Camera->SetupAttachment(Root);

	bIsInUse = false;
}

// Called when the game starts or when spawned
void ACrane::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
		bIsInUse = false;
	else
		Server_SetInUse(false);
	Direction = -1;
	CurrentLength = 0.f;
}

// Called every frame
void ACrane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (bIsLifting)
		{
			CurrentLength += -Direction * 200.f * DeltaTime;
			if (CurrentLength < MaxLength)
			{
				RelativeMagnetLocation += FVector(0.f, 0.f, Direction * 200.f * DeltaTime);
				Magnet->SetWorldLocation(RelativeMagnetLocation, true);
				//UE_LOG(LogTemp, Warning, TEXT("Magnet on local: %s"), *Magnet->GetComponentLocation().ToString());
				Attach();
				Server_Lift(Magnet->GetComponentLocation());
				if (Direction > 0.f && CurrentLength <= 0.f)
				{
					UE_LOG(LogTemp, Warning, TEXT("Lift false: %f"), CurrentLength);
					bIsLifting = false;
				}
			}

		}

	}
	if (bOverload)
		Overload(DeltaTime);
}

void ACrane::PossessedBy(AController* InController)
{
	Super::PossessedBy(InController);
	bIsInUse = true;
}

void ACrane::UnPossessed()
{
	Super::UnPossessed();
	bIsInUse = false;
}

// Called to bind functionality to input
void ACrane::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(CMoveAction, ETriggerEvent::Triggered, this, &ACrane::PerformMove);
		EIC->BindAction(CMoveAction, ETriggerEvent::Completed, this, &ACrane::FinishMove);
		EIC->BindAction(CAttachAction, ETriggerEvent::Started, this, &ACrane::StartMagnet);
		EIC->BindAction(CDropAction, ETriggerEvent::Started, this, &ACrane::Detach);
		EIC->BindAction(CExitAction, ETriggerEvent::Started, this, &ACrane::Exit);
	}
}

void ACrane::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACrane, bStartMagnet);
	DOREPLIFETIME(ACrane, bIsInUse);
	DOREPLIFETIME(ACrane, bOverload);
	//DOREPLIFETIME(ACrane, PhysicsConstraint);
}

void ACrane::Restart()
{
	Super::Restart();
	if (IsLocallyControlled())
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			if (ULocalPlayer* LP = PC->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* LPSubsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
				{
					LPSubsystem->ClearAllMappings();
					if (IMC_Crane)
					{
						LPSubsystem->AddMappingContext(IMC_Crane, 0);
						UE_LOG(LogTemp, Warning, TEXT("Crane MappingContext init by server"));
					}
					UE_LOG(LogTemp, Warning, TEXT("Crane EICSubystem init by server"));
				}
				UE_LOG(LogTemp, Warning, TEXT("Crane LP init by server"));
			}
			UE_LOG(LogTemp, Warning, TEXT("Crane PC init by server"));
		}
	}
}

void ACrane::PerformMove(const FInputActionValue& InValue)
{
	float Value = InValue.Get<float>();

	if (!bIsLifting && !bOverload)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Crane Moving Value: %f"), Value);
		AddActorWorldOffset(FVector(0.f, Speed * Value * GetWorld()->GetDeltaSeconds(), 0.f), true);
		bIsMoving = true;
		if(!HasAuthority())
		Server_Move(GetActorLocation());
	}

}

void ACrane::FinishMove(const FInputActionValue& InValue)
{
	bIsMoving = false;
}

void ACrane::StartMagnet(const FInputActionValue& InValue)
{
	bool Value = InValue.Get<bool>();
	if(!Value && !bIsMoving && !bOverload)
	{
		if (Magnet)//PhysicsConstraint)
		{
			Magnet->SetSimulatePhysics(false);
			//PhysicsConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Free, 1000.f);
			RelativeMagnetLocation = Magnet->GetComponentLocation();
			UE_LOG(LogTemp, Warning, TEXT("Gravity and physics disabled!"));
		}
		bIsLifting = true;
		CurrentLength = 0.f;
		//Magnet->AddWorldOffset(FVector(0.f, 0.f, 500.f * Value * GetWorld()->GetDeltaSeconds()), false);
		/*if (!HasAuthority())
			Server_Lift(Magnet->GetComponentLocation());*/
	}
}

void ACrane::Detach(const FInputActionValue& InValue)
{
	bool Value = InValue.Get<bool>();
	if (!Value && !bOverload)
	{
		UE_LOG(LogTemp, Warning, TEXT("DetachActor"));
		Server_DetachActor();
	}
}

void ACrane::Exit(const FInputActionValue& InValue)
{
	bool bValue = InValue.Get<bool>();
	UE_LOG(LogTemp, Warning, TEXT("Exit? : %d"), bValue);
	if (!bValue)
	{
		AZPlayerController* ZPC = Cast<AZPlayerController>(GetController());
		if (ZPC)
		{
			ZPC->SwitchPawn(nullptr);
		}
	}
}

void ACrane::Overload(float DeltaTime)
{
	if (ControlComponent)
	{
	
		FVector RelativeLocation = ControlComponent->GetRelativeLocation();
		FVector DirectionVector = OverloadLocation - RelativeLocation;
		FVector NormalizedDirectionVector = (OverloadLocation - RelativeLocation).GetSafeNormal();
		if (DirectionVector.IsNearlyZero(Speed))
		{
			bOverload = false;
			//run sim
			return;
		}
		NormalizedDirectionVector.X *= -Speed * DeltaTime;
		NormalizedDirectionVector.Y = 0.f;
		NormalizedDirectionVector.Z = 0.f;
		ControlComponent->AddWorldOffset(NormalizedDirectionVector, true);
	}
}

void ACrane::Attach()
{
	//UE_LOG(LogTemp, Warning, TEXT("Attach Run!"));
	FCollisionQueryParams CQP;
	CQP.AddIgnoredActor(this);

	FHitResult MagnetHit;
	FVector Start = Magnet->GetComponentLocation();
	FVector End = Start - FVector::UpVector * 500.f;
	if (GetWorld()->LineTraceSingleByChannel(MagnetHit, Start, End, ECC_Visibility, CQP))
	{
		if (MagnetHit.GetActor())
		{
			MagnetHit.GetActor()->SetActorEnableCollision(false);
			Server_AttachActor(MagnetHit.GetActor());
			//AttachedActor = MagnetHit.GetActor();
			Direction = 1;
		}
	}
	
}

void ACrane::Server_SetInUse_Implementation(bool bInValue)
{
	bIsInUse = bInValue;
}

void ACrane::Server_Exit_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("Server InUse"));
	bIsInUse = false;
}

void ACrane::Server_Lift_Implementation(FVector MagnetLocation)
{
	if (Magnet)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Magnet on server: %s"), *Magnet->GetComponentLocation().ToString());
		//Magnet->AddRelativeLocation(FVector(0.f, 0.f, -100.f * GetWorld()->GetDeltaSeconds()), true);
		Magnet->SetWorldLocation(MagnetLocation);
	}
	//Attach();
}

void ACrane::Server_DetachActor_Implementation()
{
	FDetachmentTransformRules DetachRules = FDetachmentTransformRules::KeepWorldTransform;
	if (AttachedActor)
	{
		AttachedActor->DetachFromActor(DetachRules);
		AttachedActor->SetActorEnableCollision(true); 
		Magnet->SetSimulatePhysics(true);
		//bOverload = true;
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("AttachedActor is nullptr"));
}

void ACrane::Server_AttachActor_Implementation(AActor* Actor)
{
	AttachedActor = Actor;
	if (Actor->AttachToComponent(Magnet, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("S_Attach")))
	{
		Actor->SetActorEnableCollision(false);
		UE_LOG(LogTemp, Warning, TEXT("Attach pass"));
	}
}

void ACrane::Server_Move_Implementation(FVector NewLocation)
{
	//UE_LOG(LogTemp, Warning, TEXT("InDelta: %f"), InDelta);
	SetActorLocation(NewLocation, true);
}

void ACrane::SetupPawnInputMappingContext(APlayerController* PC)
{
	if (PC)
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* LPSubsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				LPSubsystem->ClearAllMappings();
				if (IMC_Crane)
				{
					LPSubsystem->AddMappingContext(IMC_Crane, 0);
					//UE_LOG(LogTemp, Warning, TEXT("Crane MappingContext init by server"));
				}
				//UE_LOG(LogTemp, Warning, TEXT("Crane EICSubystem init by server"));
			}
			//UE_LOG(LogTemp, Warning, TEXT("Crane LP init by server"));
		}
		//UE_LOG(LogTemp, Warning, TEXT("Crane PC init by server"));
	}
	//UE_LOG(LogTemp, Warning, TEXT("Crane init by server"));
}

void ACrane::SetControlMesh(UStaticMeshComponent* InComponent, UStaticMeshComponent* InMagnet, UPhysicsConstraintComponent* InPCComponent)
{
	ControlComponent = InComponent;
	Magnet = InMagnet;
	PhysicsConstraint = InPCComponent;
	ControlComponent->SetIsReplicated(true);
	Magnet->SetIsReplicated(true);
	Magnet->bReplicatePhysicsToAutonomousProxy = true;
	PhysicsConstraint->SetIsReplicated(true);
}

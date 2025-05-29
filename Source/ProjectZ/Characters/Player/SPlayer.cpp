// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayer.h"
#include "SPAnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "./ProjectZ/UI/SPlayerHUD.h"
#include "Components/SceneComponent.h"
#include "./ProjectZ/Weapons/Weapon.h"
#include "Components/CapsuleComponent.h"
#include "./ProjectZ/Weapons/Ammunition.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "./ProjectZ/Weapons/WeaponComponent.h"
#include "./ProjectZ/Characters/MoveComponent.h"
#include "ProjectZ/Interfaces/InteractionInterface.h"
#include "EnhancedInput/Public/InputActionValue.h"
#include "EnhancedInput/Public/InputAction.h"
#include "ProjectZ/Quest/QuestItem.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASPlayer::ASPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArm = CREATE(USpringArmComponent, "SpringArmComponent");
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	//SKMesh->SetupAttachment(Camera);

	WComp = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponsComponent"));
	bReplicates = true;
	SetReplicates(true);
	SetReplicateMovement(true);//tbd
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	//SetReplicateMovement(true);
	//GetCharacterMovement()->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void ASPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("My Role: Server"));
	}
	else if(GetLocalRole() == ROLE_AutonomousProxy)
	{
		UE_LOG(LogTemp, Warning, TEXT("My Role: Client"));
	}

	AnimInst = Cast<USPAnimInstance>(GetMesh()->GetAnimInstance());
	InitPlayerHUD();

	GetCollider()->OnComponentBeginOverlap.AddDynamic(this, &ASPlayer::OnBeginOverlap);
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* LPSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if(IMC)
			LPSubsystem->AddMappingContext(IMC, 0);
		}
	}
}

// Called every frame
void ASPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MoveComp)
		MoveComp->UpdateMovement(DeltaTime);
	/*if (HasAuthority())
	{
		MoveUpdate(DeltaTime);
	}
	else
	{
		Server_MoveUpdate(DeltaTime);
	}*/
}

// Called to bind functionality to input
void ASPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASPlayer::PerformMove);
		
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASPlayer::Look);

		//EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		//EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EIC->BindAction(FireAction, ETriggerEvent::Started, this, &ASPlayer::Attack);
		EIC->BindAction(FireAction, ETriggerEvent::Completed, this, &ASPlayer::StopAttack);
		EIC->BindAction(FireAction, ETriggerEvent::Canceled, this, &ASPlayer::StopAttack);

		EIC->BindAction(ReloadAction, ETriggerEvent::Started, this, &ASPlayer::ReloadWeapon);

		EIC->BindAction(InteractAction, ETriggerEvent::Started, this, &ASPlayer::BeginInteract);
	}
}

void ASPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ASPlayer, SpringArm, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASPlayer, Camera, COND_OwnerOnly);
	DOREPLIFETIME(ASPlayer, LookY);
}

void ASPlayer::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor)
	{
		if (OtherActor->IsA<AAmmunition>())
		{
			AAmmunition* Ammo = Cast<AAmmunition>(OtherActor);
			if (Ammo)
			{
				if (WComp->AddAmmo(Ammo->GetDropAmmoAmount(true)))
				{
					Ammo->SetOwner(this);
					Ammo->InitDestroy();
				}
			}
		}
		else if (OtherActor->IsA<AQuestItem>())
		{
			if (!HasAuthority()) return;
			AQuestItem* Item = Cast<AQuestItem>(OtherActor);
			if (Item && Item->IsOfType(EQuestItemType::Interactable))
			{
				Item->Interact(this);
			}
		}
	}
}

void ASPlayer::PerformMove(const FInputActionValue& Value)
{	
	FVector2D Direction = Value.Get<FVector2D>();
	if (Direction.Size() > 0.01f)
	{
		FVector DirectionVector = GetActorForwardVector() * Direction.X + GetActorRightVector() * Direction.Y;
		MoveComp->AddMovement(DirectionVector, EMoveState::Walk);
	}
}

void ASPlayer::Look(const FInputActionValue& Value)
{
	FVector2D InValue = Value.Get<FVector2D>();
	LookValue += InValue;
	LookValue.Y = FMath::Clamp(LookValue.Y, -35.f, 50.f);
	
	SetActorRotation(FRotator(0.f, LookValue.X, 0.f));
	SpringArm->SetWorldRotation(FRotator(LookValue.Y, LookValue.X, 0.f));
	LookY = -LookValue.Y;
	
	if(!HasAuthority())
	Server_Look(LookValue);
}

void ASPlayer::Server_Look_Implementation(FVector2D LookVal)
{
	SetActorRotation(FRotator(0.f, LookVal.X, 0.f));
	SpringArm->SetWorldRotation(FRotator(LookVal.Y, LookVal.X, 0.f));
	LookY = -LookVal.Y;
	//UE_LOG(LogTemp, Warning, TEXT("Player: %s, LookX: %f"), *GetName(), LookY);
	//HeadComp->SetWorldRotation(FRotator(LookVal.Y, LookVal.X, 0.f));
}

bool ASPlayer::Server_Look_Validate(FVector2D LookVal)
{
	return true;
}

void ASPlayer::Attack(const FInputActionValue& Value)
{
	//bool bValue = Value.Get<bool>();
	if (WComp)
	{
		WComp->UseWeapon(true);
	}
}

void ASPlayer::StopAttack(const FInputActionValue& Value)
{
	if (WComp)
		WComp->UseWeapon(false);
}

void ASPlayer::ReloadWeapon(const FInputActionValue& Value)
{
	if (WComp)
	{
		//Server_BeginReload();
		bool bValue = WComp->ReloadWeapon();
		UE_LOG(LogTemp, Warning, TEXT("Reload : %d"), bValue);
	}
}

void ASPlayer::Server_BeginInteract_Implementation()
{
	BeginInteract();
}

void ASPlayer::Server_BeginReload_Implementation()
{
	if (AnimInst && WComp)
	{
		AWeapon* C_Weapon = WComp->GetCurrentWeapon();
		UAnimMontage* ReloadMontage = C_Weapon->GetWeaponProperties().ReloadMontage;
		AnimInst->Montage_Play(ReloadMontage);
		NetMulticast_PlayReloadMontage(ReloadMontage);
	}
}

void ASPlayer::NetMulticast_PlayReloadMontage_Implementation(UAnimMontage* InMontage)
{
	if (AnimInst)// && WComp)
	{
		AnimInst->Montage_Play(InMontage);
	}
}

void ASPlayer::Server_MoveUpdate_Implementation(float DeltaTime)
{
	MoveUpdate(DeltaTime);
}

void ASPlayer::MoveUpdate(float DeltaTime)
{
	if (MoveComp)
		MoveComp->HandleOverallMovement(DeltaTime);
	else
		UE_LOG(LogTemp, Warning, TEXT(" %s : MoveComp is null!"), *GetName());
}

void ASPlayer::BeginInteract()
{
	if (HasAuthority())
	{
		FHitResult InteractHit;
		FVector Start = Camera->GetComponentLocation();
		FVector End = Start + Camera->GetForwardVector() * InteractionRange;
		if (HasAuthority())
			UE_LOG(LogTemp, Warning, TEXT("Camera Name: %s"), *Camera->GetName());
		if (GetWorld()->LineTraceSingleByChannel(InteractHit, Start, End, ECC_Visibility))
		{
			DrawDebugLine(GetWorld(), Start, InteractHit.Location, FColor::Blue, false, 2.f);
			UE_LOG(LogTemp, Warning, TEXT("Actor Hit: %s"), *InteractHit.GetActor()->GetName());
			if (InteractHit.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
			{
				IInteractionInterface::Execute_Interact_BP(InteractHit.GetActor(), this);

				if (IInteractionInterface* InteractActor = Cast<IInteractionInterface>(InteractHit.GetActor()))
				{
					InteractActor->Interact(this);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Not InteractActor"));
			}
		}
	}
	else
		Server_BeginInteract();
}

void ASPlayer::InitPlayerHUD()
{
	if (PlayerHUDClass)
	{
		UUserWidget* Widget = CreateWidget<UUserWidget>(Cast<APlayerController>(GetController()), PlayerHUDClass);
		if (Widget)
		{
			PlayerHUD = Cast<USPlayerHUD>(Widget);
			PlayerHUD->AddToPlayerScreen();
			PlayerHUD->AddQuestDescription(CurrentQuestDescription);
			if (HDComp)
			{
				PlayerHUD->SetMaxHealth(HDComp->GetMaxHealth());
				PlayerHUD->SetCurrentHealth(HDComp->GetHealth());
			}
			UE_LOG(LogTemp, Warning, TEXT("PlayerHUD init"));
		}
	}
}

void ASPlayer::SetCurrentAmmo(int32 InCurrentAmmo)
{
	if (PlayerHUD)
		PlayerHUD->SetCurrentAmmo(InCurrentAmmo);
}

void ASPlayer::SetTotalAmmo(int32 InTotalAmmo)
{
	if (PlayerHUD)
		PlayerHUD->SetTotalAmmo(InTotalAmmo);
}

bool ASPlayer::AddWeapon(AWeapon* NewWeapon)
{
	if (WComp)
		WComp->AddWeapon(NewWeapon);
	return false;
}

void ASPlayer::UpdateAnimInstanceMontage(UAnimMontage* InMontage)
{
	if (AnimInst)
		AnimInst->UpdateAnimMontage(InMontage);
}

void ASPlayer::AddQuestDescription(FQuestDescription InDescription)
{
	if (PlayerHUD)
		PlayerHUD->AddQuestDescription(InDescription);
}

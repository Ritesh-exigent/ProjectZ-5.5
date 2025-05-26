// Fill out your copyright notice in the Description page of Project Settings.


#include "ZAISense_Sight.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectZ/Characters/AI/ZEnemy.h"
#include "../PerceptionComponent.h"

UZAISense_Sight::UZAISense_Sight()
{
}

void UZAISense_Sight::InitSense(AActor* OwningActor, UPerceptionComponent* InComponent)
{
	Super::InitSense(OwningActor, InComponent);
	bIsAlert = false;
	Enemy = Cast<AZEnemy>(InComponent->GetOwner());
}

void UZAISense_Sight::ActivateSense()
{
}

void UZAISense_Sight::DeactivateSense()
{
}

void UZAISense_Sight::UpdateSense(float DeltaTime)
{
	if (!bIsAlert)
		DetectPlayers();
}

void UZAISense_Sight::DetectPlayers()
{
	float MinDistance = 9999999999999999.f;
	FVector Pos = GetOwningEnemy()->GetActorLocation();
	if (UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Pos, SightConfig.SightRange, { UEngineTypes::ConvertToObjectType(ECC_Pawn) }, nullptr, {}, Players))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Sphere Overlapped, Radius: %f"), SightConfig.SightRange);
		//check fov
		for (int i = 0; i < Players.Num(); ++i)
		{
			if (IsDetected(Players[i]->GetActorLocation()) && FVector::Dist(GetOwningEnemy()->GetActorLocation(), Players[i]->GetActorLocation()) < MinDistance)
			{
				bIsAlert = true;
				//UE_LOG(LogTemp, Warning, TEXT("Player Detected"));
				GetPerceptionComponent()->SetTargetActor(Players[i]);
			}
		}
		//inform the task
		/*for (AActor* Player : Players)
		{

		}*/
	}
	//DrawDebugSphere(GetWorld(), Pos, SightConfig.SightRange, 12, FColor::Blue, false, 1.1f);
}

bool UZAISense_Sight::IsDetected(FVector PlayerLocation)
{
	FVector PlayerDirection = (PlayerLocation - GetOwningEnemy()->GetActorLocation()).GetSafeNormal();
	FVector ForwardDirection = GetOwningEnemy()->GetActorForwardVector();
	float DotProduct = ForwardDirection.Dot(PlayerDirection);
	if (DotProduct >= FMath::Cos(SightConfig.FOV / 2.f))
	return true;
	return false;
}

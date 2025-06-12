// Fill out your copyright notice in the Description page of Project Settings.


#include "LightManger.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SpotLightComponent.h"
#include "RoomFromKnowWhere.h"
#include "Engine/SpotLight.h"

// Sets default values
ALightManger::ALightManger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALightManger::BeginPlay()
{
	Super::BeginPlay();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALight::StaticClass(), FoundLights);
	for (AActor* Actor : FoundLights)
	{
		ALight* SpotLight = Cast<ALight>(Actor);
		if (SpotLight)
		{
			if (SpotLight->ActorHasTag("RedRoom")) {

				SpotLight->GetLightComponent()->SetIntensity(0.0f);
			}
		}
	}
}

// Called every frame
void ALightManger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALightManger::LightsToggleAll()
{
	UE_LOG(LogTemp, Warning, TEXT("Reached"));
	FTimerHandle TimerHandle;
	ToggleLight();

	TArray<AActor*> FoundObj;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomFromKnowWhere::StaticClass(), FoundObj);
	
	for (AActor* Rooms : FoundObj) {
		if (Rooms) {
			ARoomFromKnowWhere* Room = Cast<ARoomFromKnowWhere>(Rooms);
			if (Room) {

				
				if (Room->ActorHasTag("RedRoom")) {
					if (!RedRoom) {
						RedRoom = Room;
						Room->bShouldStayHidden = !Room->bShouldStayHidden;
					}
					RedRoom = Room;
					UE_LOG(LogTemp, Error, TEXT("RedRoom Flipped1"));
				}
				else {
					Room->bShouldStayHidden = !Room->bShouldStayHidden;
				}
			}
		}
	}
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ALightManger::ToggleLight, 1.0f, false);
}

void ALightManger::ToggleLight()
{

	if (RedRoom) {
		RedRoom->bShouldStayHidden = !RedRoom->bShouldStayHidden;
		UE_LOG(LogTemp, Error, TEXT("RedRoom Flipped     2"));

	}
	for (AActor* Actor : FoundLights)
	{
		ALight* SpotLight = Cast<ALight>(Actor);
		if (SpotLight)
		{
			if (SpotLight->ActorHasTag("MainLight")) {
				FTimerHandle TimerHandle;
				if (SpotLight->GetLightComponent()->Intensity == 0.0f) {
					SpotLight->GetLightComponent()->SetIntensity(160.0f);
				}
				
				else {
					SpotLight->GetLightComponent()->SetIntensity(0.0f);
				}
			}
			else if (SpotLight->ActorHasTag("RedRoom")) {
				UE_LOG(LogTemp, Error, TEXT("found red light"));
				if (SpotLight->GetLightComponent()->Intensity == 0.0f) {
					SpotLight->GetLightComponent()->SetIntensity(160.0f);
				}
				else {
					SpotLight->GetLightComponent()->SetIntensity(0.0f);

				}

			}
			/*USpotLightComponent* LightComp = Cast<USpotLightComponent>(SpotLight->GetLightComponent());
			LightComp->SetVisibility(false);*/
			/*GetWorld()->GetTimerManager().SetTimer(TimerHandle, [LightComp]()

				{
					if (LightComp)
					{
						LightComp->SetVisibility(true);

					}
				}, .6f, false);*/
		}
	}


}

void ALightManger::MainRoomInit()
{
	FTimerHandle MainTimerHandle;

	for (AActor* Actor : FoundLights)
	{
		ALight* SpotLight = Cast<ALight>(Actor);
		if (SpotLight)
		{
			if (SpotLight->ActorHasTag("MainLight")) {
		
				SpotLight->GetLightComponent()->LightFunctionMaterial = LightFunction;
				SpotLight->GetLightComponent()->MarkRenderStateDirty();
			

			}
		}
	}
	GetWorld()->GetTimerManager().SetTimer(MainTimerHandle, this, &ALightManger::MainRoomFinalize, 0.4f, false);

}

void ALightManger::MainRoomFinalize()
{
	for (AActor* Actor : FoundLights)
	{
		ALight* SpotLight = Cast<ALight>(Actor);
		if (SpotLight)
		{
			if (!SpotLight->ActorHasTag("GaurdLight") && SpotLight->ActorHasTag("MainLight")) {
				SpotLight->GetLightComponent()->SetIntensity(1.0f);
			
				SpotLight->GetLightComponent()->LightFunctionMaterial = nullptr;
				SpotLight->GetLightComponent()->MarkRenderStateDirty();
			
			}
			else if (SpotLight->ActorHasTag("GaurdLight")) {
				SpotLight->GetLightComponent()->SetIntensity(10.0f);

			
				SpotLight->GetLightComponent()->LightFunctionMaterial = nullptr;
				SpotLight->GetLightComponent()->MarkRenderStateDirty();
	
			}
		}
	}
	
}


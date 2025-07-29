// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeWidget.generated.h"

class UButton;

UCLASS()
class PROJECTZ_API UUpgradeWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual bool Initialize() override;

private:

	UPROPERTY(meta=(BindWidget))
	UButton* Btn_Health;
	
	UPROPERTY(meta=(BindWidget))
	UButton* Btn_Stamina;
	
	UPROPERTY(meta=(BindWidget))
	UButton* Btn_Weapon;
};

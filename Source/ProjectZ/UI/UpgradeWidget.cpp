// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradeWidget.h"
#include "Components/Button.h"

bool UUpgradeWidget::Initialize()
{
	bool bSuper = Super::Initialize();
	if(!bSuper || !Btn_Health || !Btn_Stamina || !Btn_Weapon)
	return false;



	return true;
}

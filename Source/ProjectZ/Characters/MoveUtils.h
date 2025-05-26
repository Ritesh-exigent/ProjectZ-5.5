// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MoveUtils.generated.h"

//UENUM()
//enum EBMoveState : uint8
//{
//	Idle = 1,
//	Walk = 2,
//	Run = 3,
//};

UENUM()
enum EMoveState : uint8
{
	Idle = 1,
	Walk = 2,
	Run = 3,
};

UENUM()
enum EMoveResult : uint8
{
	Completed = 4,
	Failed = 5,
	InProgress = 6,
	None = 7,
};

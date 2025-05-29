// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestUtils.generated.h"

class AQuestItem;

UENUM()
enum EQuestItemType : uint8
{
	Interactable,
	Pickup,
};

UENUM()
enum EQuestType {
	QT_Normal,
	QT_Collect,
	QT_Time,
};

UENUM()
enum EQuestStatus {
	QS_Pending,
	QS_Completed,
	QS_Failed,
};

USTRUCT()
struct FSubQuestInfo {
	GENERATED_BODY()
public:
	FSubQuestInfo() {
		SubQuestID = 0;
		SubQuestTitle = FText();
		SubQuestType = EQuestType::QT_Normal;
		Status = EQuestStatus::QS_Pending;
	}

	UPROPERTY(EditAnywhere)
	int32 SubQuestID;
	UPROPERTY(EditAnywhere)
	FText SubQuestTitle;
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EQuestType> SubQuestType;
	UPROPERTY(EditAnywhere, Category = "Values", meta = (EditCondition = "SubQuestType == EQuestType::QT_Collect"))
	int32 TotalCount;
	UPROPERTY(EditAnywhere, Category = "Values", meta = (EditCondition = "SubQuestType == EQuestType::QT_Time"))
	float TotalTime;
	UPROPERTY(EditAnywhere)
	TArray<TSoftObjectPtr<AQuestItem>> RequiredItems;
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EQuestStatus> Status;
};

USTRUCT()
struct FQuestInfo : public FTableRowBase {
	GENERATED_BODY()
public:
	FQuestInfo() {
		QuestTitle = FText();
		Status = EQuestStatus::QS_Pending;
	}

	UPROPERTY(EditAnywhere)
	FText QuestTitle;
	UPROPERTY(EditAnywhere)
	TArray<FSubQuestInfo> SubQuests;
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EQuestStatus> Status;
};

USTRUCT()
struct FQuestDescription
{
	GENERATED_BODY()

public:

	FQuestDescription(){}
	FQuestDescription(FString InQTitle, FString InSQTitle, int32 InTotalCount = 0, float InTotalTime = 0.f)
	{
		QTitle = FText::FromString(InQTitle);
		SQTitle = FText::FromString(InSQTitle);
		TotalCount = FText::FromString(FString::FromInt(InTotalCount));
		TotalTime = FText::FromString(FString::SanitizeFloat(InTotalTime));
	}

	static FText IntToText(int32 Num) {
		return FText::FromString(FString::FromInt(Num));
	}

	static FText FloatToText(float Num) {
		return FText::FromString(FString::SanitizeFloat(Num));
	}

	FText QTitle;
	FText SQTitle;
	FText TotalCount;
	FText TotalTime;
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GamePointsComponent.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPointsAwarded, int, PointsAmount);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAURISKATE_API UGamePointsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGamePointsComponent();

	FOnPointsAwarded OnPointsAwarded;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	int PointsAccumulated = 0;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AwardPoints(int NewPoints);
};

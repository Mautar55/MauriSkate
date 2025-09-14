// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstacleSettingsComponent.h"

#include "GamePointsComponent.h"
#include "MauriSkateCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/GameModeBase.h"

// Sets default values for this component's properties
UObstacleSettingsComponent::UObstacleSettingsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UObstacleSettingsComponent::BeginPlay()
{
	Super::BeginPlay();
	UBoxComponent* Trigger =  GetOwner()->GetComponentByClass<UBoxComponent>();
	UStaticMeshComponent* Obstacle =  GetOwner()->GetComponentByClass<UStaticMeshComponent>();

	Trigger->OnComponentBeginOverlap.AddDynamic(this,&UObstacleSettingsComponent::TriggerJustEntered);
	Trigger->OnComponentEndOverlap.AddDynamic(this,&UObstacleSettingsComponent::TriggerJustLeft);
	Obstacle->OnComponentHit.AddDynamic(this,&UObstacleSettingsComponent::ObstacleJustHit);
}

void UObstacleSettingsComponent::TriggerJustEntered(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (AMauriSkateCharacter * SkatePawn = Cast<AMauriSkateCharacter>(OtherActor)) {
		bPawnIsCrossing = true;
	}
}

void UObstacleSettingsComponent::TriggerJustLeft(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (AMauriSkateCharacter * SkatePawn = Cast<AMauriSkateCharacter>(OtherActor)) {
		if (bPawnIsCrossing) {
			bPawnIsCrossing = false;
			UGamePointsComponent * PointsManager = GetWorld()->GetFirstPlayerController()->GetComponentByClass<UGamePointsComponent>();
			PointsManager->AwardPoints(PointsToAward);
		}
	}
}

void UObstacleSettingsComponent::ObstacleJustHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (AMauriSkateCharacter * SkatePawn = Cast<AMauriSkateCharacter>(OtherActor)) {
		SkatePawn->KillCharacter();
	}
}


// Called every frame
void UObstacleSettingsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


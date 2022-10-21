// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "AlienSpider.generated.h"

UCLASS(Blueprintable)
class SPACEGAME_API AAlienSpider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAlienSpider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	int NUM_LEGS = 8;

	FTransform IKFootTrace(int legIndex, float DeltaSeconds);
	FTransform CalculateFootDestination(int legIndex, FTransform FootR, FTransform FootO, FTransform FootD, FTransform FootL);
	FTransform InterpolateFoot(int legIndex, float DeltaSeconds, FTransform FootO, FTransform FootD, FTransform FootL);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		UCapsuleComponent* m_Capsule;

	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* m_Mesh;

	UPROPERTY(EditAnywhere)
		TArray<FTransform> m_WSFootLocation;

	UFUNCTION(BlueprintCallable, category = "MyBlueprintFunctionLibrary")
		TArray<FTransform> getFootLocations();

	TArray<FTransform> m_WSFootDestination;

	TArray<FTransform> m_WSFootOrigin;

	TArray<FTransform> m_SSFootRestingPoint;

	TArray<FVector> m_SSFootOriginNormal;

	TArray<FVector> m_SSFootDestinationNormal;

	TArray<float> m_LegDeltas;

	TArray<char> m_LegState;

	TArray<float> m_LegTime;

	TArray<double> m_MaxLegExtenstionDistance;

	TArray<float> m_LegStepHeights;

	TArray<float> m_LegStepSpeeds;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

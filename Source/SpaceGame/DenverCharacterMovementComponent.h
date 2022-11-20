// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DenverCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SPACEGAME_API UDenverCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend class FSavedMove_My;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My Character Movement|Grounded", Meta = (AllowPrivateAccess = "true"))
	float RunSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My Character Movement|Grounded", Meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My Character Movement|Grounded", Meta = (AllowPrivateAccess = "true"))
	float RunAcceleration = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My Character Movement|Grounded", Meta = (AllowPrivateAccess = "true"))
	float SprintAcceleration = 3000.0f;

public:
	UFUNCTION(BlueprintCallable, Category = "My Character Movement")
	void SetSprinting(bool Sprinting);

	UFUNCTION(BlueprintCallable, Category = "My Character Movement")
	uint8 GetSprinting() const { return SprintKeyDown; };

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	virtual float GetMaxSpeed() const override;

	virtual float GetMaxAcceleration() const override;

	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

private:
	bool SprintKeyDown = false;
	uint8 WantsToSprint : 1;
};

class FSavedMove_My: public FSavedMove_Character
{
public:
	typedef FSavedMove_Character Super;

	virtual void Clear() override;

	virtual uint8 GetCompressedFlags() const override;

	virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* Character, float MaxDelta) const override;

	virtual void  SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;

	virtual void PrepMoveFor(class ACharacter* Character) override;

private:
	uint8 SavedWantsToSprint : 1;
};

class FNetworkPredictionData_Client_My : public FNetworkPredictionData_Client_Character
{
public:
	typedef FNetworkPredictionData_Client_Character Super;

	// Constructor
	FNetworkPredictionData_Client_My(const UCharacterMovementComponent& ClientMovement);

	virtual FSavedMovePtr AllocateNewMove() override;

};
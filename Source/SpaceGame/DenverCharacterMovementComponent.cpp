// Fill out your copyright notice in the Description page of Project Settings.


#include "DenverCharacterMovementComponent.h"
#include "SpaceGameCharacter.h"


FNetworkPredictionData_Client* UDenverCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		// Return our custom client prediction class instead
		UDenverCharacterMovementComponent* MutableThis = const_cast<UDenverCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_My(*this);
	}

	return ClientPredictionData;
}


void UDenverCharacterMovementComponent::SetSprinting(bool Sprinting)
{
	SprintKeyDown = Sprinting;
}

void UDenverCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// Peform local only checks
	if (GetPawnOwner()->IsLocallyControlled())
	{
		if (SprintKeyDown == true)
		{
			// Only set WantsToSprint to true if the player is moving forward (so that he can't sprint backwards)
			FVector velocity2D = GetPawnOwner()->GetVelocity();
			FVector forward2D = GetPawnOwner()->GetActorForwardVector();
			velocity2D.Z = 0.0f;
			forward2D.Z = 0.0f;
			velocity2D.Normalize();
			forward2D.Normalize();

			WantsToSprint = FVector::DotProduct(velocity2D, forward2D) > 0.5f;
		}
		else
		{
			WantsToSprint = false;
		}

	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDenverCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	WantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

float UDenverCharacterMovementComponent::GetMaxSpeed() const
{
	switch (MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
	{
		if (IsCrouching())
		{
			return MaxWalkSpeedCrouched;
		}
		else
		{
			if (WantsToSprint)
			{
				return SprintSpeed;
			}

			return RunSpeed;
		}
	}
	case MOVE_Falling:
		return RunSpeed;
	case MOVE_Swimming:
		return MaxSwimSpeed;
	case MOVE_Flying:
		return MaxFlySpeed;
	case MOVE_Custom:
		return MaxCustomMovementSpeed;
	case MOVE_None:
	default:
		return 0.0f;
	}
}

float UDenverCharacterMovementComponent::GetMaxAcceleration() const
{
	if (IsMovingOnGround())
	{
		if (WantsToSprint)
		{
			return SprintAcceleration;
		}

		return RunAcceleration;
	}

	return Super::GetMaxAcceleration();
}

uint8 FSavedMove_My::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (SavedWantsToSprint)
	{
		Result |= FLAG_Custom_0;
	}

	return Result;
}

bool FSavedMove_My::CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* Character, float MaxDelta) const
{
	const FSavedMove_My* NewMove = static_cast<const FSavedMove_My*>(NewMovePtr.Get());

	// As an optimization, check if the engine can combine saved moves.
	if (SavedWantsToSprint != NewMove->SavedWantsToSprint)
	{
		return false;
	}

	return Super::CanCombineWith(NewMovePtr, Character, MaxDelta);
}


void FSavedMove_My::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);
	UDenverCharacterMovementComponent* charMov = static_cast<UDenverCharacterMovementComponent*>(Character->GetMovementComponent());
	if (charMov)
	{
		SavedWantsToSprint = charMov->WantsToSprint;
	}
}

void FSavedMove_My::PrepMoveFor(class ACharacter* Character)
{
	Super::PrepMoveFor(Character);
	UDenverCharacterMovementComponent* charMov = static_cast<UDenverCharacterMovementComponent*>(Character->GetMovementComponent());
	if (charMov)
	{
		charMov->WantsToSprint = SavedWantsToSprint;
	}
}

void FSavedMove_My::Clear()
{
	Super::Clear();

	// Clear all values
	SavedWantsToSprint = 0;
}

FNetworkPredictionData_Client_My::FNetworkPredictionData_Client_My(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{

}

FSavedMovePtr FNetworkPredictionData_Client_My::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_My());
}
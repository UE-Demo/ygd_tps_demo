// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoAnimInstance.h"

void UDemoAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (DemoCharacter == nullptr)
	{
		DemoCharacter = Cast<ADemoCharacter>(TryGetPawnOwner());
	}

	if (DemoCharacter)
	{
		FVector Velocity{ DemoCharacter->GetVelocity() };
		CharacterSpeed = Velocity.Size2D();

		bIsInAir = DemoCharacter->GetCharacterMovement()->IsFalling();

		bIsAccelerating = DemoCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size2D() > 0.0f;
	}

	FRotator AimRotation = DemoCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(DemoCharacter->GetVelocity());
	MovementOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
}

void UDemoAnimInstance::NativeInitializeAnimation()
{
	DemoCharacter = Cast<ADemoCharacter>(TryGetPawnOwner());
}

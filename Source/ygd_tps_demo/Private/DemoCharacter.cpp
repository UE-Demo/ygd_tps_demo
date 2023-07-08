// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoCharacter.h"

// Sets default values
ADemoCharacter::ADemoCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraSpringArm->TargetArmLength = 300.f;
	CameraSpringArm->bUsePawnControlRotation = true;
	CameraSpringArm->SocketOffset = FVector(0.f, -50.f, 70.f);

	CharacterCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CharacterCamera"));
	CharacterCamera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);
	CharacterCamera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

}

// Called when the game starts or when spawned
void ADemoCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADemoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADemoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

#pragma region initEnhancedInputSystem
	// Get the player controller
	APlayerController* PC = Cast<APlayerController>(GetController());

	// Get the local player subsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	// Clear out existing mapping, and add our mapping
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(DefaultMappingContext, 0);

	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);
#pragma endregion

	// Bind the actions
	PEI->BindAction(IA_Debug, ETriggerEvent::Triggered, this, &ADemoCharacter::PEIDebug);
	PEI->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ADemoCharacter::CharacterMove);
	PEI->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ADemoCharacter::CharacterLook);
}

void ADemoCharacter::PEIDebug(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("PEIDebug"));
}

void ADemoCharacter::CharacterMove(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("SAFGFWAF"));
	FVector2D MoveValue = value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		const FRotator MovementRotation(0, Controller->GetControlRotation().Yaw, 0);

		// Forward/Backward direction
		if (MoveValue.Y != 0.f)
		{
			// Get forward vector
			const FVector Direction = MovementRotation.RotateVector(FVector::ForwardVector);

			AddMovementInput(Direction, MoveValue.Y);
		}

		// Right/Left direction
		if (MoveValue.X != 0.f)
		{
			// Get right vector
			const FVector Direction = MovementRotation.RotateVector(FVector::RightVector);

			AddMovementInput(Direction, MoveValue.X);
		}
	}
}

void ADemoCharacter::CharacterLook(const FInputActionValue& value)
{
	FVector2D LookAxis = value.Get<FVector2D>();

	float TurnScaleFactor{1.f};
	float LookUpScaleFactor{1.f};
	//if (bAiming)
	//{
	//	LookUpScaleFactor = MouseAimingLookUpRate;
	//	TurnScaleFactor = MouseAimingTurnRate;
	//}
	//else
	//{
	//	LookUpScaleFactor = MouseHipLookUpRate;
	//	TurnScaleFactor = MouseHipTurnRate;
	//}
	AddControllerYawInput(LookAxis.X * TurnScaleFactor); // turn

	AddControllerPitchInput(LookAxis.Y * LookUpScaleFactor); // look up


}


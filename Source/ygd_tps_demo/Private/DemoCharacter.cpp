// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoCharacter.h"

// Sets default values
ADemoCharacter::ADemoCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#pragma region init Camera
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
#pragma endregion

	
}

// Called when the game starts or when spawned
void ADemoCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	//FName BoneName = BarrelSocket->BoneName;
	//FName SocketName = BarrelSocket->SocketName;
	//UE_LOG(LogTemp, Warning, TEXT("Parent Bone: %s, Self Socket: %s"), *BoneName.ToString(), *SocketName.ToString());

	AnimInstance = GetMesh()->GetAnimInstance();
}

// Called every frame
void ADemoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// WeaponFire();
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
	PEI->BindAction(IA_SemiAutoWeaponFire, ETriggerEvent::Triggered, this, &ADemoCharacter::WeaponFire);
}

void ADemoCharacter::PEIDebug(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("PEIDebug"));
}

void ADemoCharacter::CharacterMove(const FInputActionValue& value)
{
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

void ADemoCharacter::WeaponFire()
{
	UE_LOG(LogTemp, Warning, TEXT("Fire"));

	if (BarrelSocket)
	{
		const FTransform BarrelSocketTransform = BarrelSocket->GetSocketLocalTransform();

		FHitResult BeamHitResult;
		bool bBeamEnd = GetBeamEndLocation(BarrelSocketTransform.GetLocation(), BeamHitResult);

		if (bBeamEnd)
		{
			// UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), )
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ImpactParticles,
				BeamHitResult.Location);

			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				BeamParticles,
				BarrelSocketTransform);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NOT BarrelSocket!"));
	}

	if (AnimInstance && HipFireMontage)
	{
		AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

bool ADemoCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult)
{
	FVector OutBeamEndLocation;
	FHitResult CrosshairHitResult;

	//bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamEndLocation);

	//if (bCrosshairHit)
	//{
	//	OutBeamEndLocation = CrosshairHitResult.Location;
	//}
	// if !bCrosshairHit, OutBeamLocation is the End location for the line trace

	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector WeaponTraceEnd{ OutBeamEndLocation };
	GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility);

	if (!OutHitResult.bBlockingHit) // object between barrel and BeamEndPoint?
	{
		OutHitResult.Location = OutBeamEndLocation;
		return false;
	}

	return true;
}

bool ADemoCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairScreenLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bWorldScreen = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairScreenLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bWorldScreen)
	{
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection };
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);

		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}

	return false;
}


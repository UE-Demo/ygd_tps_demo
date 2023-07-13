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
	CameraSpringArm->SocketOffset = FVector(0.f, -50.f, 50.f);

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

	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");

	if (BarrelSocket)
	{
		const FTransform BarrelSocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

		FVector BeamHitLocation;
		bool bBeamEnd = GetBeamEndLocation(
			BarrelSocketTransform.GetLocation(), BeamHitLocation);

		if (bBeamEnd)
		{
			// UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), )
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					BeamHitLocation);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Not ImpactParticles"));
			}

			if (BeamParticles)
			{
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					BeamParticles,
					BarrelSocketTransform);
				if (Beam)
				{
					Beam->SetVectorParameter(FName("Target"), BeamHitLocation);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Not Beam"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Not BeamParticles"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NOT bBeamEnd!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NOT BarrelSocket!"));
	}

	AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

bool ADemoCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& BeamEndLocation)
{
	// Check for crosshair trace hit
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, BeamEndLocation);

	if (bCrosshairHit)
	{
		// Tentative beam location - still need to trace from gun
		BeamEndLocation = CrosshairHitResult.Location;
	}
	else // no crosshair trace hit
	{
		// OutBeamLocation is the End location for the line trace
	}

	// Perform a second trace, this time from the gun barrel
	FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ BeamEndLocation - WeaponTraceStart };
	const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f };
	GetWorld()->LineTraceSingleByChannel(
		WeaponTraceHit,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility);
	if (WeaponTraceHit.bBlockingHit) // object between barrel and BeamEndPoint?
	{
		BeamEndLocation = WeaponTraceHit.Location;
		return true;
	}

	return false;
}

bool ADemoCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	// Get Viewport Size
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// Get screen space location of crosshairs
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// Get world position and direction of crosshairs
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		// Trace from Crosshair world location outward
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * 50'000.f };
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


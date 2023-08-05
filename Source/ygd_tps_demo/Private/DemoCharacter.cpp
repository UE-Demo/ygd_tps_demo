// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoCharacter.h"
#include "DemoItem.h"

// Sets default values
ADemoCharacter::ADemoCharacter() :
	// Aim
	bAiming(false),
	CameraAimingZoomFOV(60.f),
	AimingFOVZoomInterpSpeed(20.f),

	LookSensitivity(1.f),
	DefaultLookSensitivity(1.f),
	AimingLookSensitivity(0.25f),

	// Shoot
	bShooting(false),
	bShouldTraceForItems(false),

	bReloadingAmmo(false),

	// StartingInventory
	Ammo9mmAmount(50),
	AmmoARAmount(30)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#pragma region init Camera
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraSpringArm->TargetArmLength = 180.f;
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
	
	CameraDefaultFOV = GetCharacterCamera()->FieldOfView;
	CameraTempFOV = CameraDefaultFOV;

	InitializeAmmoAmout();

	EquipWeapon(SpawnDefaultWeapon());

}

// Called every frame
void ADemoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimingZoomInterp(DeltaTime);

	TraceForItems();
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

	// Bind the actions
	PEI->BindAction(IA_Debug, ETriggerEvent::Triggered, this, &ADemoCharacter::PEIDebug);
	PEI->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ADemoCharacter::CharacterMove);
	PEI->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ADemoCharacter::CharacterLook);

	PEI->BindAction(IA_WeaponFire, ETriggerEvent::Ongoing, this, &ADemoCharacter::WeaponFire);

	PEI->BindAction(IA_ReloadAmmo, ETriggerEvent::Triggered, this, &ADemoCharacter::ReloadAmmo);


	PEI->BindAction(IA_Aiming, ETriggerEvent::Triggered, this, &ADemoCharacter::AimTrigger);
	
	PEI->BindAction(IA_Interact, ETriggerEvent::Triggered, this, &ADemoCharacter::CharacterInteract);
	PEI->BindAction(IA_DropWeapon, ETriggerEvent::Triggered, this, &ADemoCharacter::DropWeapon);
#pragma endregion
}

void ADemoCharacter::PEIDebug(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("PEIDebug"));
	AmmoAmountMap.Add(EAmmoType::EAmmoType_9mm, *AmmoAmountMap.Find(EAmmoType::EAmmoType_9mm)+1);
}

void PEIDebugA(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("PEIDebugA"));
}

#pragma region Items
ADemoWeapon* ADemoCharacter::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		return GetWorld()->SpawnActor<ADemoWeapon>(DefaultWeaponClass);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not DefaultWeaponClass"));
		return nullptr;
	}
}

void ADemoCharacter::CharacterInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterInteract"));
	if (TraceHitItem)
	{
		if (auto TraceHitWeapon = Cast<ADemoWeapon>(TraceHitItem))
		{
			UE_LOG(LogTemp, Log, TEXT("Interact Weapon"));
			SwapWeapon(TraceHitWeapon);
		}
		else if(auto TraceHitAmmo = Cast<ADemoAmmo>(TraceHitItem))
		{
			UE_LOG(LogTemp, Log, TEXT("Interact Ammo"));
			PickUpAmmo(TraceHitAmmo);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Interact Unknown Actor"));
		}
	}
}

void ADemoCharacter::EquipWeapon(ADemoWeapon* WeaponToEquip)
{
	const USkeletalMeshSocket* RightHandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (RightHandSocket)
	{
		RightHandSocket->AttachActor(WeaponToEquip, GetMesh());
	}

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetItemState(EItemState::EItemState_Equipped);
}

void ADemoCharacter::SwapWeapon(ADemoWeapon* WeaponToSwap)
{
	DropWeapon();
	EquipWeapon(WeaponToSwap);
}

void ADemoCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTransformRules(
			EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

		// EquippedWeapon->SetItemState(EItemState::EItemState_Falling);
		EquippedWeapon->ItemStartFalling();

		EquippedWeapon = nullptr;
	}
}

void ADemoCharacter::PickUpAmmo(ADemoAmmo* PickAmmo)
{
	SetAmount(PickAmmo->GetAmmoType(), PickAmmo->GetAmmoAmount() + GetAmmoAmount(PickAmmo->GetAmmoType()));
	PickAmmo->Destroy();
}



void ADemoCharacter::InitializeAmmoAmout()
{
	AmmoAmountMap.Add(EAmmoType::EAmmoType_9mm, Ammo9mmAmount);
	AmmoAmountMap.Add(EAmmoType::EAmmoType_AR, AmmoARAmount);
}

void ADemoCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

void ADemoCharacter::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector OnlyForTempNotUsed;
		TraceUnderCrosshairs(ItemTraceResult, OnlyForTempNotUsed);
		if (ItemTraceResult.bBlockingHit)
		{
			ADemoItem* HitItem = Cast<ADemoItem>(ItemTraceResult.GetActor());

			if (HitItem && HitItem->GetDropInfoWidget())
			{
				HitItem->GetDropInfoWidget()->SetVisibility(true);
			}

			// check if TraceItem changes or become null at this frame
			if (TraceHitItem)
			{
				if (HitItem != TraceHitItem)
				{
					// close the old HitItem Widget
					TraceHitItem->GetDropInfoWidget()->SetVisibility(false);
				}
			}

			TraceHitItem = HitItem;
		}
	}
	else if (TraceHitItem) // no longer overlap any items
	{
		// close HitItem Widget
		TraceHitItem->GetDropInfoWidget()->SetVisibility(false);
		TraceHitItem = nullptr;
	}
}
#pragma endregion

#pragma region CharacterControll

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

	AddControllerYawInput(LookAxis.X * LookSensitivity); // turn
	AddControllerPitchInput(LookAxis.Y * LookSensitivity); // look up

}
#pragma endregion

#pragma region WeaponShoot

bool ADemoCharacter::CheckWeaponAmmoEmpty()
{
	if (!EquippedWeapon) { return false; }

	return { EquippedWeapon->GetAmmoAmount() == 0 };
}

void ADemoCharacter::WeaponFire()
{
	if (EquippedWeapon == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("No Equipped Weapon"));
		return;
	}
	
	if (CheckWeaponAmmoEmpty())
	{
		return;
	}

	if (bReloadingAmmo)
	{
		return;
	}

	const USkeletalMeshSocket* BarrelSocket =
		EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");

	if (EquippedWeapon->GetCanFire() && !EquippedWeapon->GetReloading())
	{
		EquippedWeapon->FireTimer();

		if (BarrelSocket)
		{
			const FTransform BarrelSocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());

			FVector BeamEndLocation;

			// 在命中点生成命中特效
			if (GetBeamEndLocation(
				BarrelSocketTransform.GetLocation(), BeamEndLocation)) // 判定命中同时获取命中信息
			{
				if (ImpactParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(
						GetWorld(),
						ImpactParticles,
						BeamEndLocation);
				}
			}

			// 弹道特效
			if (BeamParticles)
			{
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					BeamParticles,
					BarrelSocketTransform);
				if (Beam)
				{
					Beam->SetVectorParameter(FName("Target"), BeamEndLocation);
				}
			}

			// 枪声
			if (GunShotSounds)
			{
				UGameplayStatics::PlaySound2D(this, GunShotSounds);
			}

			// 射击动画蒙太奇
			AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance && HipFireMontage)
			{
				AnimInstance = GetMesh()->GetAnimInstance();
				AnimInstance->Montage_Play(HipFireMontage);
				AnimInstance->Montage_JumpToSection(FName("StartFire"));
			}

			// 弹药消耗
			if (EquippedWeapon)
			{
				EquippedWeapon->DecrementAmmoAmount(1);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No BarrelSocket On the Weapon Mesh!!!"));
		}

	}
}

void ADemoCharacter::ReloadAmmo()
{
	EAmmoType WeaponAmmoType = EquippedWeapon->GetWeaponAmmoType();

	if (EquippedWeapon->GetAmmoAmount()!= EquippedWeapon->GetBulletMaxAmmo() && 
		GetAmmoAmount(WeaponAmmoType) != 0)
		// bullet not full and inventory ammo not empty
	{
		if (EquippedWeapon->GetAmmoAmount() + GetAmmoAmount(WeaponAmmoType) >= EquippedWeapon->GetBulletMaxAmmo()) // can fill the whole weapon bullet
		{
			// minus reload bullet in inventory
			AmmoAmountMap.Add(WeaponAmmoType, GetAmmoAmount(WeaponAmmoType) + EquippedWeapon->GetAmmoAmount() - EquippedWeapon->GetBulletMaxAmmo()); 
			// fill the weapon bullet
			EquippedWeapon->ReloadAmmo(EquippedWeapon->GetBulletMaxAmmo()); 
		}
		else // can not fill the entire weapon bullet
		{
			// fill the weapon bullet
			EquippedWeapon->ReloadAmmo(EquippedWeapon->GetAmmoAmount() + *AmmoAmountMap.Find(WeaponAmmoType));
			// minus reload bullet in inventory
			AmmoAmountMap.Add(WeaponAmmoType,  0);		
		}
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
	else
	{
		return false;
	}

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
		const FVector EndIfNotHit{ CrosshairWorldPosition + CrosshairWorldDirection * 3000.f };
		// OutHitLocation = End;
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
		else
		{
			// 射程，暂时在这里定义
			float WeaponRange = 20.f;
			OutHitLocation = EndIfNotHit;
			return false;
		}
	}
	return false;
}

#pragma endregion

#pragma region WeaponAim

void ADemoCharacter::AimTrigger()
{
	if (bAiming)
	{
		bAiming = false;
		LookSensitivity = DefaultLookSensitivity;
		GetCharacterCamera()->SetFieldOfView(CameraDefaultFOV);
	}
	else
	{
		bAiming = true;
		LookSensitivity = AimingLookSensitivity;
		GetCharacterCamera()->SetFieldOfView(CameraAimingZoomFOV);
	}
}

void ADemoCharacter::AimingZoomInterp(float DeltaTime)
{
	if (bAiming)
	{
		CameraTempFOV = FMath::FInterpTo(CameraTempFOV, CameraAimingZoomFOV, DeltaTime, AimingFOVZoomInterpSpeed);
		GetCharacterCamera()->SetFieldOfView(CameraTempFOV);
	}
	else
	{
		CameraTempFOV = FMath::FInterpTo(CameraTempFOV, CameraDefaultFOV, DeltaTime, AimingFOVZoomInterpSpeed);
		GetCharacterCamera()->SetFieldOfView(CameraTempFOV);
	}
}


#pragma endregion

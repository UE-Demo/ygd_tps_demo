// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DemoItem.h"
#include "DemoWeapon.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"

// Assets
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"

#include "Kismet/GameplayStatics.h"

#include "DemoCharacter.generated.h"

UCLASS()
class YGD_TPS_DEMO_API ADemoCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADemoCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



#pragma region CameraControll
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraSpringArm;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CharacterCamera;

	FORCEINLINE UCameraComponent* GetCharacterCamera(){return CharacterCamera;}
#pragma endregion

#pragma region EnhancedInput
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* IA_Debug;

	/* Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* IA_Move;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* IA_Look;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* IA_WeaponFire;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* IA_ReloadAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* IA_Aiming;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* IA_Interact;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* IA_DropWeapon;

	void CharacterMove(const FInputActionValue& value);
	void CharacterLook(const FInputActionValue& value);

	void PEIDebug(const FInputActionValue& value);
	void PEIDebugA(const FInputActionValue& value);
#pragma endregion

#pragma region Anim
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	class UAnimInstance* AnimInstance;
#pragma endregion

#pragma region Asset
	/* Particles spawned upon bullet impact */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Assets, meta = (AllowPrivateAccess = "true"))
		UParticleSystem* ImpactParticles;

	/* Smoke trail for bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Assets, meta = (AllowPrivateAccess = "true"))
		UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Assets, meta = (AllowPrivateAccess = "true"))
	class USoundCue* GunShotSounds;
#pragma endregion

#pragma region Combat

	bool CheckWeaponAmmoEmpty();
	bool bShooting;

	/* Called when the Fire Button is pressed. */
	void WeaponFire();

	/* Return HitResult from crosshair. */
	bool GetBeamEndLocation(
		const FVector& MuzzleSocketLocation, FVector& BeamEndLocation);

	/* Line trace for items under the crosshair. */
	bool TraceUnderCrosshairs(
		FHitResult& OutHitResult,
		FVector& OutHitLocation);

	bool bAiming;

	float CameraDefaultFOV;
	float CameraAimingZoomFOV;
	/* Use for Aiming Interpolation FOV. */
	float CameraTempFOV;
	float AimingFOVZoomInterpSpeed;

	float LookSensitivity;

	/* Look rate when not aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float DefaultLookSensitivity;
	/* Look rate when aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float AimingLookSensitivity;

	void AimTrigger();
	/* Called in Tick. */
	void AimingZoomInterp(float DeltaTime);

	bool bReloadingAmmo;
	void ReloadAmmo();

#pragma endregion

#pragma region ItemCheck
private:
	bool bShouldTraceForItems;
	int8 OverlappedItemCount;
	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }
	
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ADemoItem* TraceHitItem;

public:
	/* Add/Substarct OverlappedItemCount and update bShouldTraceForItems. */
	void IncrementOverlappedItemCount(int8 Amount);

protected:
	/* Trace for itmes if OverlappedItemCount > 0. */
	void TraceForItems();
#pragma endregion

#pragma region Inventory
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class ADemoWeapon* EquippedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<ADemoWeapon> DefaultWeaponClass;

	ADemoWeapon* SpawnDefaultWeapon();

	void CharacterInteract();
	void GetPickupItem(ADemoItem* Item);

	void EquipWeapon(ADemoWeapon* WeaponToEquip);
	void SwapWeapon(ADemoWeapon* WeaponToSwap);
	void DropWeapon();

	// Ammo Set up
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoAmountMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 Start9mmAmmoAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 StartARAmmoAmount;

	void InitializeAmmoAmout();

#pragma endregion



};

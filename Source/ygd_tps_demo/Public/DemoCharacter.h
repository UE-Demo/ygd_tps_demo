// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Particles/ParticleSystemComponent.h"

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
		class UInputAction* IA_SemiAutoWeaponFire;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* IA_Aiming;

	void PEIDebug(const FInputActionValue& value);
	void CharacterMove(const FInputActionValue& value);
	void CharacterLook(const FInputActionValue& value);
#pragma endregion

#pragma region Anim
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	class UAnimInstance* AnimInstance;
#pragma endregion

#pragma region Asset
	/* Particles spawned upon bullet impact */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UParticleSystem* ImpactParticles;

	/* Smoke trail for bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UParticleSystem* BeamParticles;
#pragma endregion

#pragma region Combat

	const UParticleSystem* WeaponShootParticle;

	/* Called when the Fire Button is pressed. */
	void WeaponFire();

	/* Return HitResult from crosshair. */
	bool GetBeamEndLocation(
		const FVector& MuzzleSocketLocation, FVector& BeamEndLocation);

	/* Line trace for items under the crosshair. */
	bool TraceUnderCrosshairs(
		FHitResult& OutHitResult,
		FVector& OutHitLocation);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	float CameraDefaultFOV;
	float CameraAimingZoomFOV;
	/* Use for Aiming Interpolation FOV. */
	float CameraTempFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float FOVAimingZoomInterpSpeed;

	void AimTrigger();
	/* Called in Tick. */
	void AimingZoomInterp(float DeltaTime);

#pragma endregion

#pragma region Items
private:
	bool bShouldTraceForItems;
	int8 OverlappedItemCount;
	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

public:
	/* Add/Substarct OverlappedItemCount and update bShouldTraceForItems. */
	void IncrementOverlappedItemCount(int8 Amount);

protected:
	/* Trace for itmes if OverlappedItemCount > 0. */
	void TraceForItems();
#pragma endregion



};

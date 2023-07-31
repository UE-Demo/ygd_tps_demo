// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoItem.h"
#include "DemoWeapon.generated.h"

UENUM(BlueprintType)
enum class EAmmoType :uint8
{
	EAmmoType_9mm UMETA(DisplayName = "9mm"),
	EAmmoType_AR UMETA(DisplayName = "AR")
};

UENUM(BlueprintType)
enum class EFireMode :uint8
{
	EFireMode_Auto UMETA(DisplayName = "Auto"),
	EFireMode_SemiAuto UMETA(DisplayName = "SemiAuto")
};

UCLASS()
class YGD_TPS_DEMO_API ADemoWeapon : public ADemoItem
{
	GENERATED_BODY()
public:
	ADemoWeapon();

	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EFireMode WeaponFireMode;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	/* Interval between two fires in auto fire mode. */
	float AutoFireInterval;

	FTimerHandle AutoFireTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bCanFire;
	void ResetFireTimer();

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 AmmoAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 BulletMaxAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float ReloadTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bReloading;

	FTimerHandle ReloadTimerHandle;

	void CompleteReloadAmmo(int32 ReloadAmmoAmount);

public:
	FORCEINLINE EFireMode GetWeaponFireMode() const { return WeaponFireMode; }
	void SetWeaponFireMode(EFireMode NewWeaponFireMode);

	FORCEINLINE bool GetCanFire() const { return bCanFire; }
	FORCEINLINE float GetAutoFireInterval() const { return AutoFireInterval; }

	UFUNCTION()
	FORCEINLINE bool GetReloading() const { return bReloading; }

	FORCEINLINE int32 GetAmmoAmount() const { return AmmoAmount; }
	FORCEINLINE int32 GetBulletMaxAmmo() const { return BulletMaxAmmo; }
	FORCEINLINE float GetReloadingAmmoTime() const { return ReloadTime; }

	/* Called when fire weapon. */
	void DecrementAmmoAmount(int32 DecrementAmount);
	void ReloadAmmo(int32 ReloadAmmoAmount);

	void FireTimer();
};

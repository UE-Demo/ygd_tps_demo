// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoItem.h"
#include "DemoAmmo.h"
#include "DemoWeapon.generated.h"



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
#pragma region Asset
	/* Skeletal Mesh for the item. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Components", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* ItemMesh;

	/* Particles spawned upon bullet impact */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Assets, meta = (AllowPrivateAccess = "true"))
		UParticleSystem* WeaponImpactParticles;

	/* Smoke trail for bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Assets, meta = (AllowPrivateAccess = "true"))
		UParticleSystem* WeaponBeamParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Assets, meta = (AllowPrivateAccess = "true"))
		class USoundCue* WeaponSounds;

public:
	FORCEINLINE UParticleSystem* GetWeaponImpactParticles() const
	{
		return WeaponImpactParticles;
	}
	FORCEINLINE UParticleSystem* GetWeaponBeamParticles() const
	{
		return WeaponBeamParticles;
	}
	FORCEINLINE USoundCue* GetWeaponSounds() const
	{
		return WeaponSounds;
	}
#pragma endregion

protected:
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = EnemyStatus, meta = (AllowPrivateAccess = "true"))
		float WeaponDamage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EFireMode WeaponFireMode;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EAmmoType WeaponAmmoType;

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
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }

	FORCEINLINE float GetWeaponDamage() const { return WeaponDamage; }
	FORCEINLINE EFireMode GetWeaponFireMode() const { return WeaponFireMode; }
	void SetWeaponFireMode(EFireMode NewWeaponFireMode);

	FORCEINLINE bool GetCanFire() const { return bCanFire; }
	FORCEINLINE float GetAutoFireInterval() const { return AutoFireInterval; }

	FORCEINLINE bool GetReloading() const { return bReloading; }

	FORCEINLINE EAmmoType GetWeaponAmmoType() const { return WeaponAmmoType; }
	FORCEINLINE int32 GetAmmoAmount() const { return AmmoAmount; }
	FORCEINLINE void SetAmmoAmount(const int32 ReloadAmmo) { AmmoAmount = ReloadAmmo;}
	FORCEINLINE int32 GetBulletMaxAmmo() const { return BulletMaxAmmo; }
	FORCEINLINE float GetReloadingAmmoTime() const { return ReloadTime; }

	/* Called when fire weapon. */
	void DecrementAmmoAmount(int32 DecrementAmount);
	void ReloadAmmo(int32 ReloadAmmoAmount);

	void FireTimer();

public:
	virtual void ItemStartFalling() override;
	virtual void ItemStopFalling() override;

protected:
	virtual void SwitchItemProperty(EItemState State);
};

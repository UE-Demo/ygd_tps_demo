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

UCLASS()
class YGD_TPS_DEMO_API ADemoWeapon : public ADemoItem
{
	GENERATED_BODY()
public:
	ADemoWeapon();

	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 AmmoAmount;

public:
	FORCEINLINE int32 GetAmmoAmount() const { return AmmoAmount; }
	/* Called when fire weapon. */
	void DecrementAmmoAmount(int32 DecrementAmount);
};

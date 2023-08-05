// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoItem.h"
#include "DemoAmmo.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EAmmoType :uint8
{
	EAmmoType_9mm UMETA(DisplayName = "9mm"),
	EAmmoType_AR UMETA(DisplayName = "AR")
};

UCLASS()
class YGD_TPS_DEMO_API ADemoAmmo : public ADemoItem
{
	GENERATED_BODY()

	ADemoAmmo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* AmmoMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 AmmoAmount;

public:
	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE int32 GetAmmoAmount() const { return AmmoAmount; }
};

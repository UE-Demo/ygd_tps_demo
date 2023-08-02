// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoItem.h"
#include "DemoAmmo.generated.h"

/**
 * 
 */
UCLASS()
class YGD_TPS_DEMO_API ADemoAmmo : public ADemoItem
{
	GENERATED_BODY()

	ADemoAmmo();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* AmmoMesh;

public:
	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; }
	
};

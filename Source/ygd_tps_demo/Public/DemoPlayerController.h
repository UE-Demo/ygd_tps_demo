// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DemoPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class YGD_TPS_DEMO_API ADemoPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ADemoPlayerController();

protected:
	virtual void BeginPlay() override;

private:
	/* Reference to the HUD Overlay. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> HUDOverlayClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UUserWidget* HUDOverlay;
};

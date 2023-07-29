// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoPlayerController.h"

#include "Blueprint/UserWidget.h"

ADemoPlayerController::ADemoPlayerController()
{
}

void ADemoPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDOverlayClass)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayClass);
		HUDOverlay->AddToViewport();
		HUDOverlay->SetVisibility(ESlateVisibility::Visible);
	}

}

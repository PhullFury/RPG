// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGController.h"
#include "Blueprint/UserWidget.h"

void ARPGController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDBP != nullptr)
	{
		HUD = CreateWidget(this, HUDBP);
		if (HUD != nullptr)
		{
			HUD->AddToViewport();
		}
	}
}
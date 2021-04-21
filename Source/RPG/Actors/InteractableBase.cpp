// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableBase.h"
#include "Engine/TriggerVolume.h"
#include "Kismet/GameplayStatics.h"
#include "RPG/Characters/RPGCharacter.h"

// Sets default values
AInteractableBase::AInteractableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

// Called when the game starts or when spawned
void AInteractableBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (IArea != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("It works"));
	}

	Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	PlayerController = Player->GetController();
}

// Called every frame
void AInteractableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IArea != nullptr)
	{
		TArray<AActor*> Result;
		IArea->GetOverlappingActors(Result);
		for (AActor* Actor : Result)
		{
			if (Actor == Player)
			{
				UE_LOG(LogTemp, Warning, TEXT("Player"));
			}
			/*if (Actor->GetInstigatorController() == PlayerController)
			{
				UE_LOG(LogTemp, Warning, TEXT("Player"));
			}*/
		}
	}

}
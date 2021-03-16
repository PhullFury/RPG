// Fill out your copyright notice in the Description page of Project Settings.

#include "KunaiBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AKunaiBase::AKunaiBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collsion Component"));
	Capsule->SetCapsuleHalfHeight(KunaiHeight/2);
	Capsule->SetCapsuleRadius(KunaiRadius);
	SetRootComponent(Capsule);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Capsule);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	Movement->InitialSpeed = KunaiSpeed;
	Movement->MaxSpeed = KunaiSpeed;
	InitialLifeSpan = KunaiLifeSpan;
}

// Called when the game starts or when spawned
void AKunaiBase::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void AKunaiBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
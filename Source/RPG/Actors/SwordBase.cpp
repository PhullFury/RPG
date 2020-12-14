// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASwordBase::ASwordBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ASwordBase::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void ASwordBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASwordBase::Attack()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Attack"));
	FVector End = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * VectorEnd;
	UKismetSystemLibrary::DrawDebugCylinder(GetWorld(), GetOwner()->GetActorLocation(), End, CylinderRadius, 5, FLinearColor::Red, 2.f, 1.f);
}
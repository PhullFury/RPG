// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#define OUT

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
	CritDamage = Damage * CritMultiplier;
}

// Called every frame
void ASwordBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASwordBase::Attack()
{
	TArray<FHitResult> TraceHitResults;
	FVector TraceStart = GetOwner()->GetActorLocation();
	FVector TraceEnd = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * VectorEnd;
	FCollisionShape TraceCylinder = FCollisionShape::MakeCapsule(CylinderRadius, CylinderHeight/2);
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(this->GetOwner());

	UKismetSystemLibrary::DrawDebugCylinder(GetWorld(), GetOwner()->GetActorLocation(), TraceEnd, CylinderRadius, 5, FLinearColor::Red, 2.f, 1.f);

	bool bIsHit = GetWorld()->SweepMultiByChannel(OUT TraceHitResults, TraceStart, TraceEnd, FQuat::Identity, ECC_GameTraceChannel1, TraceCylinder, TraceParams);
	if (bIsHit)
	{
		FVector HitDirection = -GetOwner()->GetActorRotation().Vector();
		for (FHitResult HitResult : TraceHitResults)
		{
			AActor* HitActor = HitResult.GetActor();
			int32 CritRoll = FMath::RandRange(1, 100);
			if (HitActor != nullptr)
			{
				if (CritRoll <= CritChance)
				{
					UE_LOG(LogTemp, Warning, TEXT("That's a crit!!!"));
					FPointDamageEvent CritEvent(CritDamage, HitResult, HitDirection, nullptr);
					HitActor->TakeDamage(CritDamage, CritEvent, GetOwner()->GetInstigatorController(), this);
				}
				else
				{
					FPointDamageEvent HitEvent(Damage, HitResult, HitDirection, nullptr);
					HitActor->TakeDamage(Damage, HitEvent, GetOwner()->GetInstigatorController(), this);
				}
			}
		}
	}
}
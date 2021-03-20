// Fill out your copyright notice in the Description page of Project Settings.

#include "KunaiBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AKunaiBase::AKunaiBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collsion Component"));
	Capsule->SetCapsuleHalfHeight(KunaiHeight / 2);
	Capsule->SetCapsuleRadius(KunaiRadius);
	SetRootComponent(Capsule);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->OnComponentHit.AddDynamic(this, &AKunaiBase::OnHit);
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

	CritDamage = Damage * CritMultiplier;
}

// Called every frame
void AKunaiBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKunaiBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherActor != GetOwner())
	{
		FVector HitDirection = -GetOwner()->GetActorRotation().Vector();
		int32 CritRoll = FMath::RandRange(1, 100);
		UE_LOG(LogTemp, Warning, TEXT("It hit"));
		if (CritRoll <= CritChance)
		{
			UE_LOG(LogTemp, Warning, TEXT("That's a crit!!!"));
			/*AActor* Actor = Hit.GetActor();
			FPointDamageEvent CritEvent(CritDamage, Hit, HitDirection, nullptr);
			OtherActor->TakeDamage(CritDamage, CritEvent, GetOwner()->GetInstigatorController(), this);*/
			UGameplayStatics::ApplyDamage(OtherActor, CritDamage, GetOwner()->GetInstigatorController(), this, DamageType);
		}
		else
		{
			/*AActor* Actor = Hit.GetActor();
			FPointDamageEvent DamageEvent(Damage, Hit, HitDirection, nullptr);
			OtherActor->TakeDamage(Damage, DamageEvent, GetOwner()->GetInstigatorController(), this);*/
			UGameplayStatics::ApplyDamage(OtherActor, Damage, GetOwner()->GetInstigatorController(), this, DamageType);
		}
	}
}
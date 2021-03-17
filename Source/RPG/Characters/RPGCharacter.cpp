// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "RPG/Actors/KunaiBase.h"
#include "RPG/Actors/SwordBase.h"

// Sets default values
ARPGCharacter::ARPGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	KunaiSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Kunai Spawn Point"));
	KunaiSpawnPoint->SetupAttachment(GetMesh());
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

}

// Called when the game starts or when spawned
void ARPGCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	bInCombat = false;
	bIsCrouching = false;
	bIsSwinging = false;
	bUseControllerRotationYaw = false;
	bCanAttack = true;
	bIsAiming = false;
	bCanThrow = true;

	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	ComboCounter = 1;
	AttackTimer = 0.f;

	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
	if (SheatheSwordBP != nullptr)
	{
		SheatheSword = GetWorld()->SpawnActor<ASwordBase>(SheatheSwordBP);
		SheatheSword->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("SheatheSocket"));
		SheatheSword->SetOwner(this);
	}	
	if (AttackSwordBP != nullptr)
	{
		AttackSword = GetWorld()->SpawnActor<ASwordBase>(AttackSwordBP);
		AttackSword->SetOwner(this);
		AttackSword->Destroy();
	}
}

// Called every frame
void ARPGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ResetCombo();
	AttackPerm();
	SetSpeed();
}

// Called to bind functionality to input
void ARPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &ARPGCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Sideways"), this, &ARPGCharacter::MoveSideways);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ARPGCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("LookSideways"), this, &ARPGCharacter::LookSideways);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ARPGCharacter::PJump);
	PlayerInputComponent->BindAction(TEXT("Sheathe"), EInputEvent::IE_Pressed, this, &ARPGCharacter::Sheathe);
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &ARPGCharacter::Crouch);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &ARPGCharacter::StartSprint);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &ARPGCharacter::StopSprint);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &ARPGCharacter::Attack);
	PlayerInputComponent->BindAction(TEXT("Zoom"), EInputEvent::IE_Pressed, this, &ARPGCharacter::StartZoom);
	PlayerInputComponent->BindAction(TEXT("Zoom"), EInputEvent::IE_Released, this, &ARPGCharacter::StopZoom);
}

void ARPGCharacter::MoveForward(float AxisValue)
{
	if (bInCombat)
	{
		AddMovementInput(GetActorForwardVector() * AxisValue);
	}
	else
	{
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation(0, Rotation.Yaw, 0);
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, AxisValue);
	}
}

void ARPGCharacter::MoveSideways(float AxisValue)
{
	if (bInCombat)
	{
		AddMovementInput(GetActorRightVector() * AxisValue);
	}
	else
	{
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation(0, Rotation.Yaw, 0);
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, AxisValue);
	}
}

void ARPGCharacter::LookUp(float AxisValue)
{
	if (bIsAiming)
	{
		this->AddControllerPitchInput(AxisValue/AimSpeedReduc);
	}
	else
	{
		this->AddControllerPitchInput(AxisValue);
	}
}

void ARPGCharacter::LookSideways(float AxisValue)
{
	if (bIsAiming)
	{
		this->AddControllerYawInput(AxisValue/AimSpeedReduc);
	}
	else
	{
		this->AddControllerYawInput(AxisValue);
	}
}

void ARPGCharacter::Attack()
{
	if (bInCombat && !bIsAiming && !GetCharacterMovement()->IsFalling())
	{
		if (ComboCounter == 1 && bCanAttack)
		{
			AttackStuff(2, Attack1Anim);
		}
		else if (ComboCounter == 2 && bCanAttack)
		{
			AttackStuff(3, Attack2Anim);
		}
		else if (ComboCounter == 3 && bCanAttack)
		{
			AttackStuff(4, Attack3Anim);
		}
		else if (ComboCounter == 4 && bCanAttack)
		{
			AttackStuff(1, Attack4Anim);
		}
	}
	else if (!bInCombat && !bIsAiming)
	{
		Sheathe();
	}
	else if (bIsAiming && !GetCharacterMovement()->IsFalling() && bCanThrow)
	{
		if (KunaiBP != nullptr)
		{
			FVector SpawnLocation = KunaiSpawnPoint->GetComponentLocation();
			FRotator SpawnRotator = KunaiSpawnPoint->GetComponentRotation();
			Kunai = GetWorld()->SpawnActor<AKunaiBase>(KunaiBP, SpawnLocation, this->GetViewRotation());
			Kunai->SetOwner(this);
			bCanThrow = false;
			KunaiCoolDown = GetWorld()->GetTimeSeconds();
		}
	}
}

void ARPGCharacter::ResetCombo()
{
	if (GetWorld()->GetTimeSeconds() - AttackTimer > ComboTimer)
	{
		ComboCounter = 1;
	}
}

void ARPGCharacter::AttackPerm()
{
	if (GetWorld()->GetTimeSeconds() - AttackCoolDown >= 0.8)
	{
		bCanAttack = true;
		bIsSwinging = false;
		GetCharacterMovement()->MaxWalkSpeed = 395.f;
	}
	if (GetWorld()->GetTimeSeconds() - KunaiCoolDown >= 1.f)
	{
		bCanThrow = true;
	}
}

void ARPGCharacter::AttackStuff(int32 NextCounter, UAnimMontage* AttackAnim)
{
	AttackTimer = GetWorld()->GetTimeSeconds();
	AttackCoolDown = GetWorld()->GetTimeSeconds();
	ComboCounter = NextCounter;
	bCanAttack = false;
	bIsSwinging = true;
	if (AttackAnim != nullptr)
	{
		PlayAnimMontage(AttackAnim);
	}
}

ASwordBase* ARPGCharacter::GetAttackSword()
{
	return AttackSword;
}

void ARPGCharacter::PAttack()
{
	AttackSword->Attack();
}

void ARPGCharacter::StartZoom()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		bIsAiming = true;
		bUseControllerRotationYaw = true;
	}
}

void ARPGCharacter::StopZoom()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		bIsAiming = false;
		bUseControllerRotationYaw = false;
	}
}

void ARPGCharacter::Sheathe()
{
	if (bInCombat && !bIsSwinging && !bIsAiming)
	{
		bInCombat = false;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->JumpZVelocity = 600.f;
		AttackSword->Destroy();
		if (SheatheSwordBP != nullptr)
		{
			SheatheSword = GetWorld()->SpawnActor<ASwordBase>(SheatheSwordBP);
			SheatheSword->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("SheatheSocket"));
			SheatheSword->SetOwner(this);
		}
	}
	else if (!bInCombat && !bIsAiming)
	{
		bInCombat = true;
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->JumpZVelocity = 400.f;
		SheatheSword->Destroy();
		if (AttackSwordBP != nullptr)
		{
			AttackSword = GetWorld()->SpawnActor<ASwordBase>(AttackSwordBP);
			AttackSword->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
			AttackSword->SetOwner(this);
		}
	}
}

void ARPGCharacter::PJump()
{
	if (bIsCrouching)
	{
		Crouch();
	}
	else if (!bIsCrouching)
	{
		Jump();
	}
}

void ARPGCharacter::StartSprint()
{
	if (!bInCombat && !bIsCrouching && !bIsAiming)
	{
		bIsSprinting = true;
	}
}

void ARPGCharacter::StopSprint()
{
	if (!bInCombat && !bIsCrouching && !bIsAiming)
	{
		bIsSprinting = false;
	}
}

void ARPGCharacter::Crouch()
{
	if (bIsCrouching)
	{
		bIsCrouching = false;
		GetCapsuleComponent()->InitCapsuleSize(42.f, 88.f);
	}
	else if (!bIsCrouching)
	{
		bIsCrouching = true;
		GetCapsuleComponent()->InitCapsuleSize(42.f, 55.f);
	}
}

void ARPGCharacter::SetSpeed()
{
	if (bIsSwinging)
	{
		GetCharacterMovement()->MaxWalkSpeed = 0.f;
	}
	else if (bIsCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = 70.f;
	}
	else if (bInCombat)
	{
		GetCharacterMovement()->MaxWalkSpeed = 395.f;
	}
	else if (bIsSprinting && !bIsAiming)
	{
		GetCharacterMovement()->MaxWalkSpeed = 500.f;
	}
	else if (bIsAiming)
	{
		GetCharacterMovement()->MaxWalkSpeed = 100.f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
	}
}

bool ARPGCharacter::GetInCombat()
{
	return bInCombat;
}

bool ARPGCharacter::GetIsCrouching()
{
	return bIsCrouching;
}

bool ARPGCharacter::GetIsAiming()
{
	return bIsAiming;
}
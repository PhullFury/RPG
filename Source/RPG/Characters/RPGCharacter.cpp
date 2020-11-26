// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "RPG/Actors/SwordBase.h"

// Sets default values
ARPGCharacter::ARPGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FRotator SpringArmRotation(0, 0, 0);
	FVector SpringArmLocation(-5, 0, 70);
	FVector SpringArmScale(1, 1, 1);
	FTransform SpringArmTransform(SpringArmRotation, SpringArmLocation, SpringArmScale);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetRelativeTransform(SpringArmTransform);
	SpringArm->TargetArmLength = 500.f;
	SpringArm->SocketOffset.Y = 0.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

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
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	ComboCounter = 1;
	AttackTimer = 0.f;
	bCanAttack = true;
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
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookSideways"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ARPGCharacter::PJump);
	PlayerInputComponent->BindAction(TEXT("Sheathe"), EInputEvent::IE_Pressed, this, &ARPGCharacter::Sheathe);
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &ARPGCharacter::Crouch);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &ARPGCharacter::StartSprint);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &ARPGCharacter::StopSprint);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &ARPGCharacter::Attack);
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

void ARPGCharacter::Attack()
{
	if (bInCombat && !GetCharacterMovement()->IsFalling())
	{
		if (ComboCounter == 1 && bCanAttack)
		{
			AttackTimer = GetWorld()->GetTimeSeconds();
			AttackCoolDown = GetWorld()->GetTimeSeconds();
			ComboCounter = 2;
			bCanAttack = false;
			GetCharacterMovement()->MaxWalkSpeed = 0.f;
			if (Attack1Anim != nullptr)
			{
				PlayAnimMontage(Attack1Anim);
			}
		}
		else if (ComboCounter == 2 && bCanAttack)
		{
			AttackTimer = GetWorld()->GetTimeSeconds();
			AttackCoolDown = GetWorld()->GetTimeSeconds();
			ComboCounter = 3;
			bCanAttack = false;
			GetCharacterMovement()->MaxWalkSpeed = 0.f;
			if (Attack2Anim != nullptr)
			{
				PlayAnimMontage(Attack2Anim);
			}
		}
		else if (ComboCounter == 3 && bCanAttack)
		{
			AttackTimer = GetWorld()->GetTimeSeconds();
			AttackCoolDown = GetWorld()->GetTimeSeconds();
			ComboCounter = 4;
			bCanAttack = false;
			GetCharacterMovement()->MaxWalkSpeed = 0.f;
			if (Attack3Anim != nullptr)
			{
				PlayAnimMontage(Attack3Anim);
			}
		}
		else if (ComboCounter == 4 && bCanAttack)
		{
			AttackTimer = GetWorld()->GetTimeSeconds();
			AttackCoolDown = GetWorld()->GetTimeSeconds();
			ComboCounter = 1;
			bCanAttack = false;
			GetCharacterMovement()->MaxWalkSpeed = 0.f;
			if (Attack4Anim != nullptr)
			{
				PlayAnimMontage(Attack4Anim);
			}
		}
	}	
}

void ARPGCharacter::PJump()
{
	if(bIsCrouching)
	{
		Crouch();
	}
	else if (!bIsCrouching)
	{
		Jump();
	}
}

void ARPGCharacter::Sheathe()
{
	if (bInCombat)
	{
		bInCombat = false;
		bUseControllerRotationYaw = false;
		//GetCharacterMovement()->MaxWalkSpeed = 300.f;
		GetCharacterMovement()->JumpZVelocity = 600.f;
		AttackSword->Destroy();
		if (SheatheSwordBP != nullptr)
		{
			SheatheSword = GetWorld()->SpawnActor<ASwordBase>(SheatheSwordBP);
			SheatheSword->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("SheatheSocket"));
			SheatheSword->SetOwner(this);
		}
	}
	else if (!bInCombat)
	{
		bInCombat = true;
		bUseControllerRotationYaw = true;
		//GetCharacterMovement()->MaxWalkSpeed = 395.f;
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

void ARPGCharacter::StartSprint()
{
	if (!bInCombat && !bIsCrouching)
	{
		bIsSprinting = true;
		//GetCharacterMovement()->MaxWalkSpeed = 500.f;
	}
}

void ARPGCharacter::StopSprint()
{
	if (!bInCombat && !bIsCrouching)
	{
		bIsSprinting = false;
		//GetCharacterMovement()->MaxWalkSpeed = 300.f;
	}
}

void ARPGCharacter::Crouch()
{
	if (bIsCrouching)
	{
		bIsCrouching = false;
		//GetCharacterMovement()->MaxWalkSpeed = 300.f;
		GetCapsuleComponent()->InitCapsuleSize(42.f, 88.f);
	}
	else if (!bIsCrouching)
	{
		bIsCrouching = true;
		//GetCharacterMovement()->MaxWalkSpeed = 70.f;
		GetCapsuleComponent()->InitCapsuleSize(42.f, 55.f);
	}
}

void ARPGCharacter::ResetCombo()
{
	if (GetWorld()->GetTimeSeconds()-AttackTimer > ComboTimer)
	{
		ComboCounter = 1;
	}
}

void ARPGCharacter::AttackPerm()
{
	if (GetWorld()->GetTimeSeconds() - AttackCoolDown >= 0.8)
	{
		bCanAttack = true;
		GetCharacterMovement()->MaxWalkSpeed = 395.f;
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

void ARPGCharacter::SetSpeed()
{
	if (bIsCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = 70.f;
	}
	else if (bInCombat)
	{
		GetCharacterMovement()->MaxWalkSpeed = 395.f;
	}
	else if (bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = 500.f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
	}
}

ASwordBase* ARPGCharacter::GetAttackSword()
{
	return AttackSword;
}
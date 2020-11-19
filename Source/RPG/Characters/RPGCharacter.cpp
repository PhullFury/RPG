// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

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
}

// Called every frame
void ARPGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ResetCombo();
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
	if (ComboCounter == 1)
	{
		AttackTimer = GetWorld()->GetTimeSeconds();
		ComboCounter = 2;
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Attack 1"));
	}
	else if (ComboCounter == 2)
	{
		AttackTimer = GetWorld()->GetTimeSeconds();
		ComboCounter = 3;
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Attack 2"));
	}
	else if (ComboCounter == 3)
	{
		AttackTimer = GetWorld()->GetTimeSeconds();
		ComboCounter = 1;
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Attack 3"));
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
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		GetCharacterMovement()->JumpZVelocity = 600.f;
		//SpringArm->TargetArmLength = 500.f;
		//SpringArm->SocketOffset.Y = 0.f;
	}
	else if (!bInCombat)
	{
		bInCombat = true;
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->MaxWalkSpeed = 395.f;
		GetCharacterMovement()->JumpZVelocity = 400.f;
		//SpringArm->TargetArmLength = 300.f;
		//SpringArm->SocketOffset.Y = 30.f;
	}
}

void ARPGCharacter::StartSprint()
{
	if (!bInCombat && !bIsCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = 500.f;
	}
}

void ARPGCharacter::StopSprint()
{
	if (!bInCombat && !bIsCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
	}
}

void ARPGCharacter::Crouch()
{
	if (bIsCrouching)
	{
		bIsCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		GetCapsuleComponent()->InitCapsuleSize(42.f, 88.f);
	}
	else if (!bIsCrouching)
	{
		bIsCrouching = true;
		GetCharacterMovement()->MaxWalkSpeed = 70.f;
		GetCapsuleComponent()->InitCapsuleSize(42.f, 55.f);
	}
}

void ARPGCharacter::ResetCombo()
{
	if (AttackTimer > ComboTimer)
	{
		ComboCounter = 1;
	}
}
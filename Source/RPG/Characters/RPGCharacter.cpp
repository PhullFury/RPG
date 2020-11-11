// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ARPGCharacter::ARPGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
	GetCapsuleComponent()->InitCapsuleSize(42.f, 88.f);
}

// Called every frame
void ARPGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &ARPGCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Sideways"), this, &ARPGCharacter::MoveSideways);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookSideways"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Sheathe"), EInputEvent::IE_Pressed, this, &ARPGCharacter::Sheathe);
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &ARPGCharacter::Crouch);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &ARPGCharacter::StartSprint);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &ARPGCharacter::StopSprint);
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

void ARPGCharacter::Sheathe()
{
	if (bInCombat)
	{
		bInCombat = false;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
	}
	else if (!bInCombat)
	{
		bInCombat = true;
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->MaxWalkSpeed = 395.f;
	}
}

void ARPGCharacter::StartSprint()
{
	if (!bInCombat)
	{
		GetCharacterMovement()->MaxWalkSpeed = 500.f;
	}
}

void ARPGCharacter::StopSprint()
{
	if (!bInCombat)
	{
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
	}
}

void ARPGCharacter::Crouch()
{
	if (bIsCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = 70.f;
		GetCapsuleComponent()->InitCapsuleSize(42.f, 55.f);
	}
	if (!bIsCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		GetCapsuleComponent()->InitCapsuleSize(42.f, 88.f);
	}
}
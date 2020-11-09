// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ARPGCharacter::ARPGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void ARPGCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	bIsInCombat = false;
	bUseControllerRotationYaw = false;
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
}

void ARPGCharacter::MoveForward(float AxisValue)
{
	if (bIsInCombat)
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
	if (bIsInCombat)
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
	if (bIsInCombat)
	{
		bIsInCombat = false;
		bUseControllerRotationYaw = false;
	}
	else if (!bIsInCombat)
	{
		bIsInCombat = true;
		bUseControllerRotationYaw = true;
	}
}
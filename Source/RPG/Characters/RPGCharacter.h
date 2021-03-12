// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGCharacter.generated.h"

class ASwordBase;
class UAnimMontage;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class RPG_API ARPGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARPGCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* SpringArm;
	UFUNCTION(BlueprintPure)
		bool GetInCombat();
	UFUNCTION(BlueprintPure)
		bool GetIsCrouching();
	UFUNCTION(BlueprintPure)
		ASwordBase* GetAttackSword();
	UFUNCTION(BlueprintCallable)
		void PAttack();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:	
	void MoveForward(float AxisValue);
	void MoveSideways(float AxisValue);
	void LookUp(float AxisValue);
	void LookSideways(float AxisValue);
	void Sheathe();
	void Crouch();
	void Attack();
	void StartSprint();
	void StopSprint();
	void PJump();
	void ResetCombo();
	void AttackPerm();
	void SetSpeed();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		float ComboTimer = 1.4f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* Attack1Anim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* Attack2Anim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* Attack3Anim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* Attack4Anim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<ASwordBase> AttackSwordBP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<ASwordBase> SheatheSwordBP;
	UPROPERTY()
		ASwordBase* AttackSword;
	UPROPERTY()
		ASwordBase* SheatheSword;

	float AttackTimer;
	int32 ComboCounter;
	float AttackCoolDown;
	bool bCanAttack;
	bool bInCombat;
	bool bIsCrouching;
	bool bIsSprinting;
	bool bIsSwinging;
};
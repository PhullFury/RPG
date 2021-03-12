// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwordBase.generated.h"

UCLASS()
class RPG_API ASwordBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASwordBase();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void Attack();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	UPROPERTY(VisibleAnywhere)
		USceneComponent* Root;
	UPROPERTY(VisibleAnywhere)
		USkeletalMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TraceParameters", meta = (AllowPrivateAccess = "true"))
		float VectorEnd = 140.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TraceParameters", meta = (AllowPrivateAccess = "true"))
		float CylinderRadius = 25.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TraceParameters", meta = (AllowPrivateAccess = "true"))
		float CylinderHeight = 70.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (AllowPrivateAccess = "true"))
		float Damage = 35.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (AllowPrivateAccess = "true"))
		float CritMultiplier = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (AllowPrivateAccess = "true"))
		int32 CritChance = 20;

	float CritDamage;
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PracRPGCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;

struct FInputActionValue;

UCLASS()
class PRACUPROJECT_API APracRPGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APracRPGCharacter();

	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	int32 GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	int32 GetMaxHealth() const;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION()
	UUserWidget* GetOverHeadWidget() const;


protected:
	UFUNCTION()
	void Move (const FInputActionValue& Value);
	UFUNCTION()
	void Look (const FInputActionValue& Value);
	UFUNCTION()
	void StartJump (const FInputActionValue& Value);
	UFUNCTION()
	void StopJump (const FInputActionValue& Value);
	UFUNCTION()
	void StartCrouch (const FInputActionValue& Value);
	UFUNCTION()
	void StopCrouch (const FInputActionValue& Value);
	UFUNCTION()
	void StartSprint (const FInputActionValue& Value);
	UFUNCTION()
	void StopSprint (const FInputActionValue& Value);

	void BeginCrouch();
	void EndCrouch();
	
	float JumpDelay = 0.0f;
	
	virtual void Landed(const FHitResult& Hit) override;

	void UpdateOverHeadHP();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera|Components")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera|Components")
	UCameraComponent* CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI|Components")
	UWidgetComponent* OverHeadWidgetComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* OverHeadWidgetInstance = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float OriginSpeed = 600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 1000.0f;
	UPROPERTY()
	bool bIsJumping = false;
	UPROPERTY()
	bool bIsCrouching = false;

	float MaxHealth = 100.0f;

	float CurrentHealth = MaxHealth;

	virtual void OnDeath();

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;
};

inline void APracRPGCharacter::BeginCrouch()
{
	bIsCrouching = true;
	Crouch();
}

inline void APracRPGCharacter::EndCrouch()
{
	UnCrouch();
	bIsCrouching = false;
}

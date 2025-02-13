// Fill out your copyright notice in the Description page of Project Settings.


#include "PracRPGCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "PracGameState.h"
#include "PracPlayerController.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"

APracRPGCharacter::APracRPGCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 400.0f;
	SpringArmComp->SocketOffset = FVector(0.0f, 40.0f, 80.0f);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	OverHeadWidgetComp = CreateDefaultSubobject<UWidgetComponent>("OverHeadWidgetComp");
	OverHeadWidgetComp->SetupAttachment(GetMesh());
	OverHeadWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bIgnoreBaseRotation = false;
}

void APracRPGCharacter::AddHealth(int32 Amount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("Health increased to : %f"), CurrentHealth)
	UpdateOverHeadHP();
}

int32 APracRPGCharacter::GetHealth() const
{
	return CurrentHealth;
}

int32 APracRPGCharacter::GetMaxHealth() const
{
	return MaxHealth;
}

void APracRPGCharacter::OnDeath()
{
	auto* PracGameState = GetWorld() ? GetWorld()->GetGameState<APracGameState>() : nullptr;
	if (PracGameState)
	{
		PracGameState->OnGameOver();
	}
}

float APracRPGCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("Health decreased to : %f"), CurrentHealth);
	UpdateOverHeadHP();

	if (CurrentHealth <= 0.0f)
	{
		OnDeath();
	}
	
	return ActualDamage;
}

void APracRPGCharacter::UpdateOverHeadHP()
{
	if (OverHeadWidgetComp)
	{
		OverHeadWidgetInstance = OverHeadWidgetComp->GetUserWidgetObject();
		if (OverHeadWidgetInstance)
		{
			UTextBlock* HPText = Cast<UTextBlock>(OverHeadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP")));
			if (HPText)
			{
				HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), CurrentHealth, MaxHealth)));
			}
		}
	}
}

UUserWidget* APracRPGCharacter::GetOverHeadWidget() const
{
	return OverHeadWidgetInstance;
}

void APracRPGCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 초기화를 위해서 SetText 진행
	UpdateOverHeadHP();
}

void APracRPGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	JumpDelay += DeltaTime;
}

void APracRPGCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MoveInput = Value.Get<FVector2D>();

	if (MoveInput.X != 0 || MoveInput.Y != 0)
	{
		// Controller 기준으로 ForwardVector 와 RightVector 를 구하는 로직
		const FRotator ControllerRotation = GetControlRotation();
		const FRotator ControllerYawRotation = FRotator(0, ControllerRotation.Yaw, 0);
		const FRotator ControllerYawAndRollRotation = FRotator(0, ControllerRotation.Yaw, ControllerRotation.Roll);

		const FVector DesiredForwardDirection = FRotationMatrix(ControllerYawRotation).GetUnitAxis(EAxis::X);
		const FVector DesiredRightDirection = FRotationMatrix(ControllerYawAndRollRotation).GetUnitAxis(EAxis::Y);
		
		const FVector MoveDirectionValue = DesiredForwardDirection*MoveInput.X + DesiredRightDirection*MoveInput.Y;
		const FVector MoveDirection = MoveDirectionValue.GetSafeNormal();
		AddMovementInput(MoveDirection);
	}
}

void APracRPGCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();

	if (LookInput.X != 0 || LookInput.Y != 0)
	{
		AddControllerYawInput(LookInput.X);
		AddControllerPitchInput(LookInput.Y);
	}
}

void APracRPGCharacter::StartJump(const FInputActionValue& Value)
{
	const bool JumpInput = Value.Get<bool>();
	
	if (JumpInput)
	{
		// 점프 딜레이 기준은 랜딩 애니메이션의 발이 땅에 닿는데 걸리는 시간
		if (JumpDelay >= 0.15f)
		{
			if (bIsCrouching)
			{
				EndCrouch();
			}
			Jump();
			bIsJumping = true;
		}
	}
}

void APracRPGCharacter::StopJump(const FInputActionValue& Value)
{
	const bool JumpInput = Value.Get<bool>();

	if (!JumpInput)
	{
		StopJumping();
		bIsJumping = false;
	}
}

void APracRPGCharacter::StartCrouch(const FInputActionValue& Value)
{
	const bool CrouchInput = Value.Get<bool>();

	if (CrouchInput && !GetCharacterMovement()->IsFalling() && !bIsJumping)
	{
		BeginCrouch();
	}
}

void APracRPGCharacter::StopCrouch(const FInputActionValue& Value)
{
	const bool CrouchInput = Value.Get<bool>();

	if (!CrouchInput)
	{
		EndCrouch();
	}
}

void APracRPGCharacter::StartSprint(const FInputActionValue& Value)
{
	const bool SprintInput = Value.Get<bool>();

	if (SprintInput)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void APracRPGCharacter::StopSprint(const FInputActionValue& Value)
{
	const bool SprintInput = Value.Get<bool>();

	if (!SprintInput)
	{
		GetCharacterMovement()->MaxWalkSpeed = OriginSpeed;
	}
}

void APracRPGCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	// 땅에 닿을 시 점프 딜레이 카운트 시작
	JumpDelay = 0.0f;
}



void APracRPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// BindAction 을 위한 EnhanceInputComponent 와 PlayerController 선언
	auto* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComp)
	{
		return;
	}

	auto* PlayerController = Cast<APracPlayerController>(GetController());
	if (!PlayerController)
	{
		return;
	}

	// Input Binding Action
	if (PlayerController->MoveAction)
	{
		EnhancedInputComp->BindAction(
			PlayerController->MoveAction,
			ETriggerEvent::Triggered,
			this,
			&APracRPGCharacter::Move
			);
	}

	if (PlayerController->LookAction)
	{
		EnhancedInputComp->BindAction(
			PlayerController->LookAction,
			ETriggerEvent::Triggered,
			this,
			&APracRPGCharacter::Look
			);
	}

	if (PlayerController->JumpAction)
	{
		EnhancedInputComp->BindAction(
			PlayerController->JumpAction,
			ETriggerEvent::Triggered,
			this,
			&APracRPGCharacter::StartJump
			);

		EnhancedInputComp->BindAction(
			PlayerController->JumpAction,
			ETriggerEvent::Completed,
			this,
			&APracRPGCharacter::StopJump
			);
	}

	if (PlayerController->CrouchAction)
	{
		EnhancedInputComp->BindAction(
			PlayerController->CrouchAction,
			ETriggerEvent::Triggered,
			this,
			&APracRPGCharacter::StartCrouch
			);
		
		EnhancedInputComp->BindAction(
			PlayerController->CrouchAction,
			ETriggerEvent::Completed,
			this,
			&APracRPGCharacter::StopCrouch
			);
	}

	if (PlayerController->SprintAction)
	{
		EnhancedInputComp->BindAction(
			PlayerController->SprintAction,
			ETriggerEvent::Triggered,
			this,
			&APracRPGCharacter::StartSprint
			);

		EnhancedInputComp->BindAction(
			PlayerController->SprintAction,
			ETriggerEvent::Completed,
			this,
			&APracRPGCharacter::StopSprint
			);
	}
}




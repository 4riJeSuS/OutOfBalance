// Copyright Epic Games, Inc. All Rights Reserved.

#include "OutOfBalanceCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "OutOfBalance.h"

#include "CollectableItem.h"
#include "Lock.h"

AOutOfBalanceCharacter::AOutOfBalanceCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AOutOfBalanceCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOutOfBalanceCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AOutOfBalanceCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AOutOfBalanceCharacter::Look);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AOutOfBalanceCharacter::Interact);
	}
	else
	{
		UE_LOG(LogOutOfBalance, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AOutOfBalanceCharacter::Interact()
{
	FVector ActorForward = GetActorForwardVector();
	// 1. Direção da Câmara com INCLINAÇÃO para o chão
	FRotator CameraRotation = FollowCamera->GetComponentRotation();
	// Alteramos o Pitch para -15.0f (ou -20.0f) para a esfera descer em direção ao chão
	FRotator InclinedRotation = FRotator(-25.0f, CameraRotation.Yaw, 0.0f);
	FVector InteractionDirection = InclinedRotation.Vector();
	// 2. Verificação de Ângulo (Dot Product)
	// Comparamos o Ator com a direção da câmara (usamos a achatada para o Dot ser mais justo)
	FVector FlatCameraForward = FRotator(0.0f, CameraRotation.Yaw, 0.0f).Vector();
	float CosAngle = FVector::DotProduct(ActorForward, FlatCameraForward);
	if (CosAngle < 0.2f)
	{
		UE_LOG(LogTemp, Display, TEXT("Não podes interagir de costas!"));
		return;
	}
	// 3. Configuração do Start e End
	// Start sai do peito (Z+60) e End usa a direção inclinada
	FVector start = GetActorLocation() + FVector(0.0f, 0.0f, 60.0f) + (ActorForward * 40.0f);
	FVector end = start + (InteractionDirection * maxInteractionDistance);
	DrawDebugLine(GetWorld(), start, end, FColor::Green, false, 5.0f);

	FCollisionShape interactionSphere = FCollisionShape::MakeSphere(interactionSphereRadius);
	DrawDebugSphere(GetWorld(), end, interactionSphereRadius, 20, FColor::Red, false, 5.0f);

	FHitResult hitResult;
	bool hasHit = GetWorld()->SweepSingleByChannel(
		hitResult, 
		start, 
		end, 
		FQuat::Identity, 
		ECC_GameTraceChannel2,
		interactionSphere
	);

	if (hasHit) 
	{
		AActor* hitActor = hitResult.GetActor();

		if (hitActor->ActorHasTag("CollectableItem")) {
			ACollectableItem* collectableItem = Cast<ACollectableItem>(hitActor);
			if (collectableItem) {
				inventory.Add(collectableItem->itemName);

				collectableItem->Destroy();
			}
		} else if (hitActor->ActorHasTag("Lock")) {
			ALock* lockActor = Cast<ALock>(hitActor);
			if (lockActor) {
				if (!lockActor->getIsKeyPlaced()) {
					int32 itemsRemoved = inventory.RemoveSingle(lockActor->keyItemName);
					if (itemsRemoved) {
						lockActor->setIsKeyPlaced(true);
					} else {
						UE_LOG(LogTemp, Display, TEXT("Key item not in inventory."));
					}
				}else {
					inventory.Add(lockActor->keyItemName);
					lockActor->setIsKeyPlaced(false);
				}
			}
		}
	} else {
		UE_LOG(LogTemp, Display, TEXT("No actor hit."));
	}
}


void AOutOfBalanceCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AOutOfBalanceCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AOutOfBalanceCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AOutOfBalanceCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AOutOfBalanceCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AOutOfBalanceCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

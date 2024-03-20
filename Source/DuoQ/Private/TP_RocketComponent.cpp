// Fill out your copyright notice in the Description page of Project Settings.


#include "TP_RocketComponent.h"
#include "DuoQ/DuoQProjectile.h"

#include "Kismet/GameplayStatics.h"

void UTP_RocketComponent::Fire()
{
	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			// APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			FRotator SpawnRotation;
			// if(PlayerController)
			// 	SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// else
			// {
			APawn* Pawn = Cast<APawn>(GetOwner());
			SpawnRotation = Pawn->GetController()->GetControlRotation();
			// }
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + FVector(100.0f,0.0f,10.0f);
	
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	
			// Spawn the projectile at the muzzle
			World->SpawnActor<ADuoQProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetOwner()->GetActorLocation());
	}
}

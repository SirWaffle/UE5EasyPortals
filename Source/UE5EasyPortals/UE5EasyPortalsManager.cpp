// Fill out your copyright notice in the Description page of Project Settings.
#include "UE5EasyPortalsManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/CharacterMovementComponent.h>

DEFINE_LOG_CATEGORY(LogPortal);

UE5EasyPortalsManager::UE5EasyPortalsManager()
{
	// Defaults.
	performantPortals = true;
	checkDirection = false;
	portalUpdateRate = 0.1f;
	maxPortalRenderDistance = 500.0f;
}

void UE5EasyPortalsManager::BeginPlay()
{
	// Set off timer to update the portals in the world.
	//timer gives up after a while? huh?
	if (performantPortals)
	{
		//FTimerDelegate timer;
		//timer.BindUFunction(this, "UpdatePortals");
		//GetWorld()->GetTimerManager().SetTimer(portalsTimer, timer, portalUpdateRate, true);
	}
}

bool UE5EasyPortalsManager::IsInFrustum(AActor* Actor)
{
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (LocalPlayer != nullptr && LocalPlayer->ViewportClient != nullptr && LocalPlayer->ViewportClient->Viewport)
	{
		FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
			LocalPlayer->ViewportClient->Viewport,
			GetWorld()->Scene,
			LocalPlayer->ViewportClient->EngineShowFlags)
			.SetRealtimeUpdate(true));

		FVector ViewLocation;
		FRotator ViewRotation;
		FSceneView* SceneView = LocalPlayer->CalcSceneView(&ViewFamily, ViewLocation, ViewRotation, LocalPlayer->ViewportClient->Viewport);
		if (SceneView != nullptr)
		{
			return SceneView->ViewFrustum.IntersectSphere(
				Actor->GetActorLocation(), Actor->GetSimpleCollisionRadius());
		}
	}

	return false;
}

void UE5EasyPortalsManager::Tick(float DeltaTime)
{
	static float nextUpdate = portalUpdateRate;
	nextUpdate -= DeltaTime;
	if (nextUpdate < 0)
	{
		nextUpdate = portalUpdateRate;
		UpdatePortals();
	}
}

void UE5EasyPortalsManager::UpdatePortals()
{
	if (GetWorld() == nullptr)
		return;

	// Save reference to the player and all portals in the scene.
	//lets try to grab waht is needed...
	if (playerCameraManager == nullptr)
	{
		playerCameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	}

	if (playerCharacter == nullptr)
	{
		playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	}

	if (playerCharacter == nullptr || playerCameraManager == nullptr)
		return;

	// Get all portals in the scene.
	static int intLastActive = 0;
	int totalActive = 0;

	for (TActorIterator<AActor> portal(GetWorld(), APortal::StaticClass()); portal; ++portal)
	{
		APortal* foundPortal = Cast<APortal>(*portal);

		// Get portal info.
		FVector portalLoc = foundPortal->GetActorLocation();
		FVector portalNorm = -1 * foundPortal->GetActorForwardVector();

		// Get the pawns facing direction.
		FVector pawnLoc = playerCameraManager->GetCameraLocation(); //cam pos
		FVector pawnDirection = FRotationMatrix(playerCameraManager->GetCameraRotation()).GetScaledAxis(EAxis::X); //cam forward
		FVector portalDirection = portalLoc - pawnLoc;

		// Get the angle difference in their directions. In Degrees.
		float angleDifference = FMath::Abs(FMath::Acos(FVector::DotProduct(pawnDirection, portalNorm))) * (180.0f / PI);
		//UE_LOG(LogTemp, Warning, TEXT("Angle Diff: %f"), angleDifference);

		// Get distance from portal.
		float portalDistance = FMath::Abs(portalDirection.Size());
		float angleDiffAmount = portalDistance <= 1000.0f ? 130.0f : 90.0f;

		// Activate the portals based on distance, if the camera is in-front and facing...
		// NOTE: This is only an example of how the portals can be made less of an impact to performance.
		// NOTE: It would be better to find a way of checking if portals are being rendered.
		// to take it further you could check recursions to see if the portal actually needs to recurse itself.
		bool looking = checkDirection ? angleDifference < angleDiffAmount : true;

		bool shouldEnable = foundPortal->IsInfront(pawnLoc) && looking && portalDistance <= maxPortalRenderDistance;

		//if we are checking with a raycast, do that here
		if (frustumCheck && shouldEnable)
		{
			//WJ_STUB
			shouldEnable = IsInFrustum(foundPortal);
		}

		if (shouldEnable)
		{
			if (foundPortal->active == false)
			{
				foundPortal->SetActive(true);
				UE_LOG(LogTemp, Warning, TEXT("Portal %s is now ACTIVE"), *(foundPortal->GetName()));
			}
			++totalActive;
		}
		else
		{
			if (foundPortal->active == true)
			{
				foundPortal->SetActive(false);
				UE_LOG(LogTemp, Warning, TEXT("Portal %s is now DISABLED"), *(foundPortal->GetName()));
			}
		}
	}

	if (totalActive != intLastActive)
		UE_LOG(LogTemp, Warning, TEXT("Total Portals active:  %d"), totalActive);
	intLastActive = totalActive;
}
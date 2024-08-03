// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Portal.h"
#include "GameFramework/GameModeBase.h"
#include "UE5EasyPortalsManager.generated.h"


/* Manages updating portals and when to set them active and inactive. */
UCLASS(Blueprintable, BlueprintType)
class UE5EASYPORTALS_API UE5EasyPortalsManager: public UObject
{
	GENERATED_BODY()

public:
	
	/* Should the game mode class activate/deactivate portals based on player location and distance. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	bool performantPortals;

	/* Should check the direction of the portals relative to the way the player is looking. 
	 * NOTE: Doesn't work that well but with more work relative to how far right the player is its possible to improve this. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	bool checkDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	bool frustumCheck;

	/* Check for portals being rendered on screen every portalUpdateRate seconds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	float portalUpdateRate;

	/* Max distance to render a portal at. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	float maxPortalRenderDistance;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Portal")
	APlayerCameraManager* playerCameraManager;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Portal")
	ACharacter* playerCharacter;

	/* Timer handle for the portals udpate function. */
	FTimerHandle portalsTimer;

public:

	/* Constructor. */
	UE5EasyPortalsManager();

	/* Function to update portals in the world based off player location relative to each of them. */
	UFUNCTION(Category = "Portals")
	void UpdatePortals();

	void Tick(float DeltaTime);
	
	bool IsInFrustum(AActor* Actor);

	/* Level start. */
	void BeginPlay();
};

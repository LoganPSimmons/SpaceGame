// Fill out your copyright notice in the Description page of Project Settings.


#include "AlienSpider.h"

// Sets default values
AAlienSpider::AAlienSpider()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Initialize Capsule collision
	
	m_Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = m_Capsule;

	//Initialize skelital mesh
	m_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(
		TEXT("SkeletalMesh'/Game/FirstPerson/AlienSpiderJoined'"));
	USkeletalMesh* Asset = MeshAsset.Object;
	m_Mesh->SetSkeletalMesh(Asset);
	m_Mesh->SetupAttachment(m_Capsule);

	//initialize leg arrays
	m_WSFootLocation.Init(FTransform(), NUM_LEGS);
	m_WSFootDestination.Init(FTransform(), NUM_LEGS);
	m_WSFootOrigin.Init(FTransform(), NUM_LEGS);
	m_SSFootRestingPoint.Init(FTransform(), NUM_LEGS);
	m_SSFootOriginNormal.Init(FVector(0,1,0), NUM_LEGS);
	m_SSFootDestinationNormal.Init(FVector(), NUM_LEGS);
	m_LegDeltas.Init(0.1f, NUM_LEGS);
	m_LegState.Init('S', NUM_LEGS);
	m_LegTime.Init(0.0f, NUM_LEGS);
	m_MaxLegExtenstionDistance.Init(400.0f, NUM_LEGS);
	m_LegStepHeights.Init(0.5f, NUM_LEGS);
	m_LegStepSpeeds.Init(3.0f, NUM_LEGS);
}

// Called when the game starts or when spawned
void AAlienSpider::BeginPlay()
{
	Super::BeginPlay();

	//Set resting points
	m_SSFootRestingPoint[0] = FTransform(FRotator(), FVector(800, -400, -100), FVector());
	m_SSFootRestingPoint[1] = FTransform(FRotator(), FVector(800, 400, -100), FVector());
	m_SSFootRestingPoint[2] = FTransform(FRotator(), FVector(500, -500, -100), FVector());
	m_SSFootRestingPoint[3] = FTransform(FRotator(), FVector(500, 500, -100), FVector());
	m_SSFootRestingPoint[4] = FTransform(FRotator(), FVector(200, -450, -100), FVector());
	m_SSFootRestingPoint[5] = FTransform(FRotator(), FVector(200, 450, -100), FVector());
	m_SSFootRestingPoint[6] = FTransform(FRotator(), FVector(-100, -300, -100), FVector());
	m_SSFootRestingPoint[7] = FTransform(FRotator(), FVector(-100, 300, -100), FVector());
	
	//set initial locations for feet
	for (int i = 0; i < NUM_LEGS; i++)
	{
		m_WSFootDestination[i] = m_SSFootRestingPoint[i] * m_Capsule->GetComponentTransform();
	}
}

// Called every frame
void AAlienSpider::Tick(float DeltaTime)
{
	
	SetActorLocation(GetActorLocation()+ FVector(1.0f, 0.0f, 0.0f));

	Super::Tick(DeltaTime);

	for (int i = 0; i < NUM_LEGS; i++)
	{
		m_WSFootLocation[i] = IKFootTrace(i, DeltaTime);
	}
}

TArray<FTransform> AAlienSpider::getFootLocations()
{
	return m_WSFootLocation;
}

// Called to bind functionality to input
void AAlienSpider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FTransform AAlienSpider::IKFootTrace(int legIndex, float DeltaSeconds)
{
	FTransform FootR = m_SSFootRestingPoint[legIndex];
	FTransform FootD = m_WSFootDestination[legIndex];
	FTransform FootL = m_WSFootLocation[legIndex];
	FTransform FootO = m_WSFootOrigin[legIndex];

	//Continue interpolation if leg is in the air
	if (m_LegState[legIndex] != 'S')
	{
		return InterpolateFoot(legIndex, DeltaSeconds, FootO, FootD, FootL);
	}

	//Check if leg is overextended, return destination point if not 
	double scaledMaxExtenstion = m_MaxLegExtenstionDistance[legIndex] * m_Mesh->GetComponentScale().X;
	double currentLegExtenstion = (FootD.GetLocation() - (m_Mesh->GetComponentLocation() + FootR.GetLocation())).Size();
	if (currentLegExtenstion < scaledMaxExtenstion) 
	{
		return FootD;
	}

	m_LegState[legIndex] = 'M';				//Set leg State to moving
	m_WSFootOrigin[legIndex] = FootD;		//Store the old destination as the origin
	m_SSFootOriginNormal[legIndex] = m_SSFootDestinationNormal[legIndex];
	m_WSFootDestination[legIndex] = CalculateFootDestination(legIndex, FootR, FootO, FootD, FootL);		//Calculate new foot destination

	return InterpolateFoot(legIndex, DeltaSeconds, FootO, FootD, FootL);
}

FTransform AAlienSpider::CalculateFootDestination(int legIndex, FTransform FootR, FTransform FootO, FTransform FootD, FTransform FootL)
{
	

	//Calculate x,y coordinates for step
	FootD = FootR * m_Mesh->GetComponentTransform();
	FootD.SetLocation(FootD.GetLocation() + (m_LegDeltas[legIndex] * GetVelocity() * m_Capsule->GetComponentScale().X));

	//Adjust z coordinate for terrain
	return FootD;
}

FTransform AAlienSpider::InterpolateFoot(int legIndex, float DeltaSeconds, FTransform FootO, FTransform FootD, FTransform FootL)
{
	//update time leg has been in the air
	/*
	m_LegTime[legIndex] = m_LegTime[legIndex] + ((m_LegStepSpeeds[legIndex] / m_Mesh->GetComponentScale().X) * DeltaSeconds);

	if (m_LegTime[legIndex] >= 1.0f)
	{
		m_LegState[legIndex] = 'S';
		m_LegTime[legIndex] = 0;
		return FootD;
	}

	FVector footReach = FootD.GetLocation() - FootO.GetLocation();
	FVector footReachScale = footReach * m_LegTime[legIndex];

	FVector footArc = (FMath::Sin(m_LegTime[legIndex] * 180) * m_LegStepHeights[legIndex] * footReach.Size()) * m_SSFootOriginNormal[legIndex];
	FootL.SetLocation(footReachScale + FootO.GetLocation() + footArc); */
	return FootL;
}


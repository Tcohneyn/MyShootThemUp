// Shoot Them Up Game,All Right Reserved.


#include "UI/STUGameHUD.h"
#include "Engine/Canvas.h"

void ASTUGameHUD::DrawHUD() 
{
	Super::DrawHUD();
	DrawCrossHair();
}

void ASTUGameHUD::DrawCrossHair() 
{
	int32 SizeX=Canvas->SizeX;
	int32 SizeY=Canvas->SizeY;
    const TInterval<float> Center(SizeX * 0.5f, SizeY * 0.5f);

	const float HalfLineSize=10.0f;
	const float LineThickness=2.0f;
    const FLinearColor LineColor = FLinearColor::Green;
    DrawLine(Center.Min-HalfLineSize,Center.Max,Center.Min+HalfLineSize,Center.Max, LineColor, LineThickness);
    DrawLine(Center.Min,Center.Max-HalfLineSize,Center.Min,Center.Max+HalfLineSize, LineColor, LineThickness);
}

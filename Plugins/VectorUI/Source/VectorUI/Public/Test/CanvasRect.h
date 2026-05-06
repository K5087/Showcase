// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CanvasWidget.h"
#include "include/core/SkPoint.h"
#include "CanvasRect.generated.h"


USTRUCT(BlueprintType)
struct VECTORUI_API FCanvasRectCorners
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	float TopLeft = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	float TopRight = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	float BotRight = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	float BotLeft = 0;

	FORCEINLINE bool IsNearlyEqual(const FCanvasRectCorners& other) const
	{
		return FMath::IsNearlyEqual(other.TopLeft, TopLeft)
			&& FMath::IsNearlyEqual(other.TopRight, TopRight)
			&& FMath::IsNearlyEqual(other.BotRight, BotRight)
			&& FMath::IsNearlyEqual(other.BotLeft, BotLeft);
	}

	FORCEINLINE bool operator==(const FCanvasRectCorners& other) const
	{
		return other.TopLeft == TopLeft
			&& other.TopRight == TopRight
			&& other.BotRight == BotRight
			&& other.BotLeft == BotLeft;
	}

	FORCEINLINE bool operator!=(const FCanvasRectCorners& other) const
	{
		return !(operator==(other));
	}
};


USTRUCT(BlueprintType)
struct VECTORUI_API FCanvasRectBorders
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	float Left = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	float Top = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	float Right = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	float Bottom = 0;

	FORCEINLINE bool IsNearlyEqual(const FCanvasRectBorders& other) const
	{
		return FMath::IsNearlyEqual(other.Left, Left)
			&& FMath::IsNearlyEqual(other.Top, Top)
			&& FMath::IsNearlyEqual(other.Right, Right)
			&& FMath::IsNearlyEqual(other.Bottom, Bottom);
	}

	FORCEINLINE bool operator==(const FCanvasRectBorders& other) const
	{
		return other.Left == Left
			&& other.Top == Top
			&& other.Right == Right
			&& other.Bottom == Bottom;
	}

	FORCEINLINE bool operator!=(const FCanvasRectBorders& other) const
	{
		return !(operator==(other));
	}
};

UCLASS()
class VECTORUI_API UCanvasRect : public UCanvasWidget
{
	GENERATED_BODY()
public:
	void OnWidgetRebuilt() override;

protected:
	void OnDraw(SkCanvas& canvas, FInt32Vector2 size, FVector2f scale) override;

private:
	using Self = UCanvasRect;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), Category = "Appearance",
		BlueprintGetter = "FillColor",
		BlueprintSetter = "SetFillColor")
	FColor _FillColor = FColor::Transparent;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), Category = "Appearance",
		BlueprintGetter = "CornerRadius",
		BlueprintSetter = "SetCornerRadius")
	FCanvasRectCorners _CornerRadius;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), Category = "Appearance",
		BlueprintGetter = "BorderColor",
		BlueprintSetter = "SetBorderColor")
	FColor _BorderColor = FColor::Transparent;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), Category = "Appearance",
		BlueprintGetter = "BorderWidth",
		BlueprintSetter = "SetBorderWidth")
	FCanvasRectBorders _BorderWidth;

public:
	UFUNCTION(BlueprintGetter) FColor FillColor() const { return _FillColor; }
	UFUNCTION(BlueprintGetter) const FCanvasRectCorners& CornerRadius() const { return _CornerRadius; }
	UFUNCTION(BlueprintGetter) FColor BorderColor() const { return _BorderColor; }
	UFUNCTION(BlueprintGetter) const FCanvasRectBorders& BorderWidth() const { return _BorderWidth; }

	UFUNCTION(BlueprintSetter) void SetFillColor(FColor in_fillColor);
	UFUNCTION(BlueprintSetter) void SetCornerRadius(const FCanvasRectCorners& in_cornerRadius);
	UFUNCTION(BlueprintSetter) void SetBorderColor(FColor in_borderColor);
	UFUNCTION(BlueprintSetter) void SetBorderWidth(const FCanvasRectBorders& in_borderWidth);

	SkVector MakeInnerRadius(float outerRadius, float bx, float by, FVector2f scale);
};

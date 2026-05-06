// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/CanvasRect.h"
#include "SCanvasWidget.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkRRect.h"
#include "include/core/SkPathBuilder.h"

void UCanvasRect::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();

	SlateWidget->OnDraw.BindUObject(this, &Self::OnDraw);
}

void UCanvasRect::OnDraw(SkCanvas& canvas, FInt32Vector2 size, FVector2f scale)
{
	float width = size.X;
	float height = size.Y;
	SkVector skScale{ scale.X, scale.Y };

	SkColor fillColor = _FillColor.ToPackedARGB();
	SkColor borderColor = _BorderColor.ToPackedARGB();

	// Scale outer radii
	SkVector outerRadii[4]{
		skScale * _CornerRadius.TopLeft,
		skScale * _CornerRadius.TopRight,
		skScale * _CornerRadius.BotRight,
		skScale * _CornerRadius.BotLeft,
	};

	// Compute inner radii
	SkVector innerRadii[4]{
		MakeInnerRadius(_CornerRadius.TopLeft, _BorderWidth.Left, _BorderWidth.Top, scale),
		MakeInnerRadius(_CornerRadius.TopRight, _BorderWidth.Right, _BorderWidth.Top, scale),
		MakeInnerRadius(_CornerRadius.BotRight, _BorderWidth.Right, _BorderWidth.Bottom, scale),
		MakeInnerRadius(_CornerRadius.BotLeft, _BorderWidth.Left, _BorderWidth.Bottom, scale),
	};

	// Prepare paint and canvas
	SkPaint paint;
	paint.setAntiAlias(true);
	paint.setStyle(SkPaint::kFill_Style);

	canvas.clear(SK_ColorTRANSPARENT);

	// Create the outer shape
	SkRRect outer;
	outer.setRectRadii(SkRect::MakeWH(width, height), outerRadii);

	// Create the inner shape
	auto innerBounds = SkRect::MakeXYWH(
		_BorderWidth.Left * scale.X,
		_BorderWidth.Top * scale.Y,
		width - (_BorderWidth.Left + _BorderWidth.Right) * scale.X,
		height - (_BorderWidth.Top + _BorderWidth.Bottom) * scale.Y);

	SkRRect inner;
	inner.setRectRadii(innerBounds, innerRadii);

	// Paint the fill color
	paint.setColor(fillColor);
	canvas.drawRRect(outer, paint);

	// Construct a path for the border area
	SkPathBuilder builder;
	builder.addRRect(outer);
	builder.addRRect(inner, SkPathDirection::kCCW);
	SkPath border = builder.detach();

	// Paint the border color
	paint.setColor(borderColor);
	canvas.drawPath(border, paint);
}

SkVector UCanvasRect::MakeInnerRadius(float outerRadius, float bx, float by, FVector2f scale)
{
	return {
		FMath::Max((outerRadius - bx) * scale.X, 0),
		FMath::Max((outerRadius - by) * scale.Y, 0),
	};
}

void UCanvasRect::SetFillColor(FColor in_fillColor)
{
	if (_FillColor == in_fillColor)
		return;

	_FillColor = in_fillColor;

	if (SlateWidget.IsValid())
		SlateWidget->RequestRedraw();
}

void UCanvasRect::SetCornerRadius(const FCanvasRectCorners& in_cornerRadius)
{
	if (_CornerRadius.IsNearlyEqual(in_cornerRadius))
		return;

	_CornerRadius = in_cornerRadius;

	if (SlateWidget.IsValid())
		SlateWidget->RequestRedraw();
}

void UCanvasRect::SetBorderColor(FColor in_borderColor)
{
	if (_BorderColor == in_borderColor)
		return;

	_BorderColor = in_borderColor;

	if (SlateWidget.IsValid())
		SlateWidget->RequestRedraw();
}

void UCanvasRect::SetBorderWidth(const FCanvasRectBorders& in_borderWidth)
{
	if (_BorderWidth.IsNearlyEqual(in_borderWidth))
		return;

	_BorderWidth = in_borderWidth;

	if (SlateWidget.IsValid())
		SlateWidget->RequestRedraw();
}

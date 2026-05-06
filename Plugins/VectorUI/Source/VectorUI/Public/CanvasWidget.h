// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"

#include "CanvasWidget.generated.h"

class SCanvasWidget;
class SkCanvas;

UCLASS(Abstract)
class VECTORUI_API UCanvasWidget : public UWidget
{
	GENERATED_BODY()

public:
	void SynchronizeProperties() override;
	void ReleaseSlateResources(bool) override;

protected:
	TSharedRef<SWidget> RebuildWidget() override;

	virtual void OnDraw(SkCanvas& canvas, FInt32Vector2 size, FVector2f scale){}
	TSharedPtr<SCanvasWidget> SlateWidget;
};

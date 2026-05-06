// Fill out your copyright notice in the Description page of Project Settings.


#include "CanvasWidget.h"

#include "SCanvasWidget.h"
#include "include/core/SkCanvas.h"

void UCanvasWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	TSharedPtr<SWidget> safeWidget = GetCachedWidget();
	if (safeWidget.IsValid())
	{
		auto* widget = (SCanvasWidget*)safeWidget.Get();
		widget->RequestRedraw();
	}
	else if (SlateWidget.IsValid())
	{
		SlateWidget->RequestRedraw();
	}
}

void UCanvasWidget::ReleaseSlateResources(bool)
{
	SlateWidget.Reset();
}

TSharedRef<SWidget> UCanvasWidget::RebuildWidget()
{
	SlateWidget = SNew(SCanvasWidget)
		.OnDraw_UObject(this, &UCanvasWidget::OnDraw);

	return SlateWidget.ToSharedRef();
}
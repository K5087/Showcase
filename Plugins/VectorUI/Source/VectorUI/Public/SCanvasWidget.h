#pragma once

#include "CoreMinimal.h"

class SkCanvas;
class SkSurface;

namespace CanvasWidget {
	class FRenderResources :public FDeferredCleanupInterface, public FGCObject
	{
	public:
		FRenderResources() = default;
		void Initialize();
		UTextureRenderTarget2D* RenderTarget() const;
		FString GetReferencerName() const override;
		void AddReferencedObjects(FReferenceCollector& gc) override;

		SkSurface& Surface() const;

		/** @returns true if the surface needs to be re-drawn. */
		bool SetDimensions(uint32 width, uint32 height, FVector2f scale);

		bool IsDrawable() const;
		void Update();
	private:
		UTextureRenderTarget2D* m_RenderTarget = nullptr;
		TSharedPtr<SkSurface> m_Surface = nullptr;
		TArray<uint8> pxBuffer;

		uint32 m_Width = 0;
		uint32 m_Height = 0;
		FVector2f m_Scale{ 1.f, 1.f };

		FThreadSafeBool m_Locked = false;
	};
}

DECLARE_DELEGATE_ThreeParams(FOnDraw,
	SkCanvas&,
	FInt32Vector2/* size */,
	FVector2f/* scale */)

class VECTORUI_API SCanvasWidget : public SLeafWidget
{
	SLATE_DECLARE_WIDGET(SCanvasWidget, SLeafWidget)
public:
	SLATE_BEGIN_ARGS(SCanvasWidget) {}
		SLATE_EVENT(FOnDraw, OnDraw)
		SLATE_ATTRIBUTE(TOptional<FVector2D>, desiredSizeOverride)
	SLATE_END_ARGS()

	SCanvasWidget();
	~SCanvasWidget();

	void Construct(const FArguments& args);

	int32 OnPaint(
		const FPaintArgs& arg,
		const FGeometry& geo,
		const FSlateRect& rect,
		FSlateWindowElementList& elements,
		int32 layer,
		const FWidgetStyle& style,
		bool parentEnabled)const override;

	void RequestRedraw();

	FOnDraw OnDraw;
protected:
	FVector2D ComputeDesiredSize(float)const override;
private:
	mutable bool needs_Redraw = false;
	mutable FSlateBrush brush;
	TSlateAttribute<TOptional<FVector2D>> desiredSizeOverride;
	CanvasWidget::FRenderResources* RenderResources = nullptr;
};

#include "SCanvasWidget.h"
#include "Engine/TextureRenderTarget2D.h"
#include "SkiaUtil.h"
#include "include\core\SkSurface.h"
#include "include\core\SkImageInfo.h"


namespace CanvasWidget {
	void FRenderResources::Initialize()
	{
		if (m_RenderTarget)
			return;
		auto* rt = NewObject<UTextureRenderTarget2D>();
		rt->ClearColor = FLinearColor::Transparent;
		rt->RenderTargetFormat = RTF_RGBA8_SRGB;
		m_RenderTarget = rt;
	}

	UTextureRenderTarget2D* FRenderResources::RenderTarget()const {
		return m_RenderTarget;
	}

	FString FRenderResources::GetReferencerName()const {
		return "CanvasWidget::FRenderResources";
	}

	void FRenderResources::AddReferencedObjects(FReferenceCollector& gc) {
		gc.AddReferencedObject(m_RenderTarget);
	}

	SkSurface& FRenderResources::Surface() const {
		check(m_Surface.IsValid());
		return *m_Surface;
	}

	bool FRenderResources::SetDimensions(uint32 width, uint32 height, FVector2f scale) {
		if (m_Locked)
		{
			// There are RHI commands for the render target already in-flight, so we
			// need to defer any updates until after they're complete.
			return false;
		}

		bool needsRedraw = false;
		if (m_Width != width || m_Height != height) {
			needsRedraw = true;
			m_Width = width;
			m_Height = height;

			if (m_RenderTarget->GameThread_GetRenderTargetResource() && m_RenderTarget->OverrideFormat == PF_B8G8R8A8) {
				m_RenderTarget->ResizeTarget(width, height);
			}
			else {
				m_RenderTarget->InitCustomFormat(width, height, PF_B8G8R8A8, false);
				m_RenderTarget->UpdateResourceImmediate();
			}

			if (m_Surface.IsValid())
				m_Surface.Reset();

			auto imageInfo = SkImageInfo::Make(width, height, kBGRA_8888_SkColorType, kUnpremul_SkAlphaType);
			size_t bufferSize = imageInfo.computeByteSize(imageInfo.minRowBytes());
			pxBuffer.SetNumZeroed(bufferSize);

			m_Surface = Skia::MakeShareable(
				SkSurfaces::WrapPixels(
					imageInfo,
					pxBuffer.GetData(),
					imageInfo.minRowBytes()
				)
			);


		}
		if (!FMath::IsNearlyEqual(m_Scale.X, scale.X)
			|| !FMath::IsNearlyEqual(m_Scale.Y, scale.Y))
		{
			needsRedraw = true;
			m_Scale = scale;
		}

		return needsRedraw;
	}

	bool FRenderResources::IsDrawable()const {
		return (m_RenderTarget != nullptr
			&& m_Surface.IsValid()
			&& !pxBuffer.IsEmpty()
			&& m_Width > 0
			&& m_Height > 0);
	}
	void FRenderResources::Update() {
		FThreadSafeBool& locked = m_Locked;
		locked.AtomicSet(true);

		UTextureRenderTarget2D* rt = m_RenderTarget;
		FUpdateTextureRegion2D region{ 0, 0, 0, 0, m_Width, m_Height };
		size_t srcPitch = m_Surface->imageInfo().minRowBytes();
		uint8* pxData = pxBuffer.GetData();

		ENQUEUE_RENDER_COMMAND(UpdateCanvasBuffer)(
			[region, rt, srcPitch, pxData, &locked](FRHICommandList&)
			{
				{
					FRHITexture* tex2d = rt
						->GetRenderTargetResource()
						->GetTextureRHI()
						->GetTexture2D();

					RHIUpdateTexture2D(tex2d, 0, region, srcPitch, pxData);
				}

				locked.AtomicSet(false);
			});
	}
}



SLATE_IMPLEMENT_WIDGET(SCanvasWidget)

void SCanvasWidget::PrivateRegisterAttributes(FSlateAttributeInitializer& init) {
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(init,
		"DesiredSizeOverride",
		desiredSizeOverride,
		EInvalidateWidgetReason::Layout);
}

SCanvasWidget::SCanvasWidget() :desiredSizeOverride(*this), RenderResources(new CanvasWidget::FRenderResources) {}
SCanvasWidget::~SCanvasWidget() {
	BeginCleanup(RenderResources);
}

void SCanvasWidget::Construct(const FArguments& args)
{
	OnDraw = args._OnDraw;
	desiredSizeOverride.Assign(*this, args._desiredSizeOverride);
	RenderResources->Initialize();
	brush.SetResourceObject(RenderResources->RenderTarget());
}

void SCanvasWidget::RequestRedraw() {
	needs_Redraw = true;
}

FVector2D SCanvasWidget::ComputeDesiredSize(float) const {
	if (const auto& sizeOverride = desiredSizeOverride.Get()) {
		return *sizeOverride;
	}
	return FVector2D::ZeroVector;
}

int32 SCanvasWidget::OnPaint(const FPaintArgs& args, const FGeometry& geo, const FSlateRect& rect, FSlateWindowElementList& elements, int32 layer, const FWidgetStyle& style, bool parentEnable)const {
	FPaintGeometry paintGeo = geo.ToPaintGeometry();
	FSlateRenderTransform transform = paintGeo.GetAccumulatedRenderTransform();

	FVector2f scale = transform.GetMatrix().GetScale().GetVector();
	FVector2f localSize = (paintGeo.GetLocalSize());
	FIntPoint renderSize = (localSize * scale).IntPoint();

	uint32 rtWidth = FMath::Abs(renderSize.X);
	uint32 rtHeight = FMath::Abs(renderSize.Y);

	if (FMath::Max(rtWidth, rtHeight) > GetMax2DTextureDimension()) {
		UE_LOG(LogTemp, Error,
			TEXT("The requested size for SCanvasWidget is too large: %i x %i"),
			rtWidth, rtHeight);

		return layer;
	}

	if (rtWidth == 0 || rtHeight == 0)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("The requested size for SCanvasWidget has a zero dimension: %i x %i"),
			rtWidth, rtHeight);

		return layer;
	}

	if (RenderResources->SetDimensions(rtWidth, rtHeight, scale)) {
		brush.ImageSize = FVector2D(rtWidth, rtHeight);
		needs_Redraw = true;
	}

	if (needs_Redraw
		&& RenderResources->IsDrawable()
		&& OnDraw.IsBound())
	{
		SkSurface& surf = RenderResources->Surface();
		SkCanvas& canvas = *surf.getCanvas();
		FInt32Vector2 size{ surf.width(), surf.height() };

		OnDraw.Execute(canvas, size, scale);
		RenderResources->Update();
		needs_Redraw = false;
	}

	FSlateDrawElement::MakeBox(elements, layer, paintGeo, &brush);

	return layer + 1;
}
#pragma once

#include "CoreMinimal.h"
#include "include/core/SkRefCnt.h"

namespace Skia {

	template <typename T>
	FORCEINLINE TSharedPtr<T> MakeShareable(sk_sp<T> ptr)
	{
		T* raw = ptr.release();
		return ::MakeShareable<T>(raw);
	}

} // namespace Skia
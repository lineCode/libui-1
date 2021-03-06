/*
 * Copyright 2013 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "sk_tool_utils.h"
#include "SkBitmapSource.h"
#include "SkColorFilterImageFilter.h"
#include "SkColorMatrixFilter.h"
#include "SkTileImageFilter.h"
#include "gm.h"

#define WIDTH 400
#define HEIGHT 100
#define MARGIN 12

namespace skiagm {

class TileImageFilterGM : public GM {
public:
    TileImageFilterGM() : fInitialized(false) {
        this->setBGColor(0xFF000000);
    }

protected:
    virtual SkString onShortName() {
        return SkString("tileimagefilter");
    }

    void make_bitmap() {
        fBitmap.allocN32Pixels(50, 50);
        SkCanvas canvas(fBitmap);
        canvas.clear(0xFF000000);
        SkPaint paint;
        paint.setAntiAlias(true);
        sk_tool_utils::set_portable_typeface(&paint);
        paint.setColor(0xD000D000);
        paint.setTextSize(SkIntToScalar(50));
        const char* str = "e";
        canvas.drawText(str, strlen(str), SkIntToScalar(10), SkIntToScalar(45), paint);
    }

    virtual SkISize onISize() {
        return SkISize::Make(WIDTH, HEIGHT);
    }

    virtual void onDraw(SkCanvas* canvas) {
        if (!fInitialized) {
            make_bitmap();

            fCheckerboard.allocN32Pixels(80, 80);
            SkCanvas checkerboardCanvas(fCheckerboard);
            sk_tool_utils::draw_checkerboard(&checkerboardCanvas, 0xFFA0A0A0, 0xFF404040, 8);

            fInitialized = true;
        }
        canvas->clear(0x00000000);

        int x = 0, y = 0;
        for (size_t i = 0; i < 4; i++) {
            SkBitmap* bitmap = (i & 0x01) ? &fCheckerboard : &fBitmap;
            SkRect srcRect = SkRect::MakeXYWH(SkIntToScalar(bitmap->width()/4),
                                              SkIntToScalar(bitmap->height()/4),
                                              SkIntToScalar(bitmap->width()/(i+1)),
                                              SkIntToScalar(bitmap->height()/(i+1)));
            SkRect dstRect = SkRect::MakeXYWH(SkIntToScalar(i * 8),
                                              SkIntToScalar(i * 4),
                                              SkIntToScalar(bitmap->width() - i * 12),
                                              SkIntToScalar(bitmap->height()) - i * 12);
            SkAutoTUnref<SkImageFilter> tileInput(SkBitmapSource::Create(*bitmap));
            SkAutoTUnref<SkImageFilter> filter(
                SkTileImageFilter::Create(srcRect, dstRect, tileInput));
            canvas->save();
            canvas->translate(SkIntToScalar(x), SkIntToScalar(y));
            SkPaint paint;
            paint.setImageFilter(filter);
            canvas->drawBitmap(fBitmap, 0, 0, &paint);
            canvas->restore();
            x += bitmap->width() + MARGIN;
            if (x + bitmap->width() > WIDTH) {
                x = 0;
                y += bitmap->height() + MARGIN;
            }
        }

        SkScalar matrix[20] = { SK_Scalar1, 0, 0, 0, 0,
                                0, SK_Scalar1, 0, 0, 0,
                                0, 0, SK_Scalar1, 0, 0,
                                0, 0, 0, SK_Scalar1, 0 };

        SkRect srcRect = SkRect::MakeWH(SkIntToScalar(fBitmap.width()),
                                        SkIntToScalar(fBitmap.height()));
        SkRect dstRect = SkRect::MakeWH(SkIntToScalar(fBitmap.width() * 2),
                                        SkIntToScalar(fBitmap.height() * 2));
        SkAutoTUnref<SkImageFilter> tile(SkTileImageFilter::Create(srcRect, dstRect, NULL));
        SkAutoTUnref<SkColorFilter> cf(SkColorMatrixFilter::Create(matrix));

        SkAutoTUnref<SkImageFilter> cfif(SkColorFilterImageFilter::Create(cf, tile.get()));
        SkPaint paint;
        paint.setImageFilter(cfif);
        canvas->save();
        canvas->translate(SkIntToScalar(x), SkIntToScalar(y));
        canvas->clipRect(dstRect);
        canvas->saveLayer(&dstRect, &paint);
        canvas->drawBitmap(fBitmap, 0, 0);
        canvas->restore();
        canvas->restore();
    }
private:
    typedef GM INHERITED;
    SkBitmap fBitmap, fCheckerboard;
    bool fInitialized;
};

//////////////////////////////////////////////////////////////////////////////

static GM* MyFactory(void*) { return new TileImageFilterGM; }
static GMRegistry reg(MyFactory);

}


/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include "SampleCode.h"
#include "SkView.h"
#include "SkCanvas.h"
#include "SkGradientShader.h"
#include "SkPath.h"
#include "SkRegion.h"
#include "SkShader.h"
#include "SkUtils.h"
#include "SkComposeShader.h"
#include "Sk1DPathEffect.h"
#include "SkCornerPathEffect.h"
#include "SkPathMeasure.h"
#include "SkRandom.h"
#include "SkColorPriv.h"
#include "SkColorFilter.h"
#include "SkLayerRasterizer.h"

#include "SkCanvasDrawable.h"

#include "SkParsePath.h"
static void testparse() {
    SkRect r;
    r.set(0, 0, 10, 10.5f);
    SkPath p, p2;
    SkString str, str2;

    p.addRect(r);
    SkParsePath::ToSVGString(p, &str);
    SkParsePath::FromSVGString(str.c_str(), &p2);
    SkParsePath::ToSVGString(p2, &str2);
}

#include "SkPictureRecorder.h"
static void test_pictbounds(SkCanvas* canvas) {
    SkRect r = SkRect::MakeXYWH(100, 50, 100, 100);
    SkPictureRecorder recorder;
    {
        SkCanvas* c = recorder.beginRecording(r, NULL, 0);
        c->drawOval(r, SkPaint());

        SkIRect ir;
        c->getClipDeviceBounds(&ir);
        SkDebugf("devbounds [%d %d %d %d]\n", ir.left(), ir.top(), ir.right(), ir.bottom());

        SkASSERT(!c->quickReject(r));
    }
    SkPicture* pic = recorder.endRecording();

    canvas->drawPicture(pic);
    SkASSERT(pic->cullRect() == r);
    pic->unref();
}

class ArcsView : public SampleView {
    class MyDrawable : public SkCanvasDrawable {
        SkRect   fR;
        SkScalar fSweep;
    public:
        MyDrawable(const SkRect& r) : fR(r), fSweep(0) {
        }

        void setSweep(SkScalar sweep) {
            if (fSweep != sweep) {
                fSweep = sweep;
                this->notifyDrawingChanged();
            }
        }

        void onDraw(SkCanvas* canvas) SK_OVERRIDE {
            SkPaint paint;
            paint.setAntiAlias(true);
            paint.setStrokeWidth(SkIntToScalar(2));

            paint.setStyle(SkPaint::kFill_Style);
            paint.setColor(0x800000FF);
            canvas->drawArc(fR, 0, fSweep, true, paint);

            paint.setColor(0x800FF000);
            canvas->drawArc(fR, 0, fSweep, false, paint);

            paint.setStyle(SkPaint::kStroke_Style);
            paint.setColor(SK_ColorRED);
            canvas->drawArc(fR, 0, fSweep, true, paint);

            paint.setStrokeWidth(0);
            paint.setColor(SK_ColorBLUE);
            canvas->drawArc(fR, 0, fSweep, false, paint);
        }

        SkRect onGetBounds() SK_OVERRIDE {
            SkRect r(fR);
            r.outset(2, 2);
            return r;
        }
    };

public:
    SkRect fRect;
    MyDrawable* fDrawable;

    ArcsView() {
        testparse();
        fSweep = SkIntToScalar(100);
        this->setBGColor(0xFFDDDDDD);

        fRect.set(0, 0, SkIntToScalar(200), SkIntToScalar(200));
        fRect.offset(SkIntToScalar(20), SkIntToScalar(20));
        fDrawable = SkNEW_ARGS(MyDrawable, (fRect));
    }

    virtual ~ArcsView() SK_OVERRIDE {
        fDrawable->unref();
    }

protected:
    // overrides from SkEventSink
    virtual bool onQuery(SkEvent* evt) {
        if (SampleCode::TitleQ(*evt)) {
            SampleCode::TitleR(evt, "Arcs");
            return true;
        }
        return this->INHERITED::onQuery(evt);
    }

    static void drawRectWithLines(SkCanvas* canvas, const SkRect& r, const SkPaint& p) {
        canvas->drawRect(r, p);
        canvas->drawLine(r.fLeft, r.fTop, r.fRight, r.fBottom, p);
        canvas->drawLine(r.fLeft, r.fBottom, r.fRight, r.fTop, p);
        canvas->drawLine(r.fLeft, r.centerY(), r.fRight, r.centerY(), p);
        canvas->drawLine(r.centerX(), r.fTop, r.centerX(), r.fBottom, p);
    }

    static void draw_label(SkCanvas* canvas, const SkRect& rect,
                            int start, int sweep) {
        SkPaint paint;

        paint.setAntiAlias(true);
        paint.setTextAlign(SkPaint::kCenter_Align);

        SkString    str;

        str.appendS32(start);
        str.append(", ");
        str.appendS32(sweep);
        canvas->drawText(str.c_str(), str.size(), rect.centerX(),
                         rect.fBottom + paint.getTextSize() * 5/4, paint);
    }

    static void drawArcs(SkCanvas* canvas) {
        SkPaint paint;
        SkRect  r;
        SkScalar w = SkIntToScalar(75);
        SkScalar h = SkIntToScalar(50);

        r.set(0, 0, w, h);
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::kStroke_Style);

        canvas->save();
        canvas->translate(SkIntToScalar(10), SkIntToScalar(300));

        paint.setStrokeWidth(SkIntToScalar(1));

        static const int gAngles[] = {
            0, 360,
            0, 45,
            0, -45,
            720, 135,
            -90, 269,
            -90, 270,
            -90, 271,
            -180, -270,
            225, 90
        };

        for (size_t i = 0; i < SK_ARRAY_COUNT(gAngles); i += 2) {
            paint.setColor(SK_ColorBLACK);
            drawRectWithLines(canvas, r, paint);

            paint.setColor(SK_ColorRED);
            canvas->drawArc(r, SkIntToScalar(gAngles[i]),
                            SkIntToScalar(gAngles[i+1]), false, paint);

            draw_label(canvas, r, gAngles[i], gAngles[i+1]);

            canvas->translate(w * 8 / 7, 0);
        }

        canvas->restore();
    }

    virtual void onDrawContent(SkCanvas* canvas) {
        if (true) { test_pictbounds(canvas); return; }

        fDrawable->setSweep(SampleCode::GetAnimScalar(SkIntToScalar(360)/24,
                                                      SkIntToScalar(360)));

        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setStrokeWidth(SkIntToScalar(2));
        paint.setStyle(SkPaint::kStroke_Style);

        drawRectWithLines(canvas, fRect, paint);

        canvas->EXPERIMENTAL_drawDrawable(fDrawable);

        drawArcs(canvas);
        this->inval(NULL);
    }

    virtual SkView::Click* onFindClickHandler(SkScalar x, SkScalar y,
                                              unsigned modi) {
     //   fSweep += SK_Scalar1;
        this->inval(NULL);
        return this->INHERITED::onFindClickHandler(x, y, modi);
    }

    virtual bool onClick(Click* click) {
        return this->INHERITED::onClick(click);
    }

private:
    SkScalar fSweep;

    typedef SampleView INHERITED;
};

//////////////////////////////////////////////////////////////////////////////

static SkView* MyFactory() { return new ArcsView; }
static SkViewRegister reg(MyFactory);

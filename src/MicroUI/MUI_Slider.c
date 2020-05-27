#include "../../include/MicroUI/MUI_Slider.h"
#include "../../include/MicroEngine/ME_Utility.h"

MUI_Slider MUI_CreateSlider(int x, int y, float currentValue, float maxValue, float minValue) {
    MUI_Slider slider;
    slider.value = currentValue;
    slider.minValue = minValue;
    slider.maxValue = maxValue;

    slider.borderRect.x = x;
    slider.borderRect.y = y;
    slider.borderRect.w = 200;
    slider.borderRect.h = 30;

    slider.slidingRect.x = x;
    slider.slidingRect.y = y;
    slider.slidingRect.h = slider.borderRect.h;

    slider.sliderHandleRect.y = slider.slidingRect.y;
    slider.sliderHandleRect.w = 10;
    slider.sliderHandleRect.h = slider.borderRect.h;

    return slider;
}

bool MUI_OnSliderValueChange(MUI_Slider *slider, SDL_Event event) {
    bool valueChanged = false;
    bool sliderHandleActive = false;
    SDL_Point mousePos;
    int x = 0;

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        mousePos.x = event.motion.x;
        mousePos.y = event.motion.y;

        x = event.motion.xrel;

        if (SDL_PointInRect(&mousePos, &slider->sliderHandleRect)) {
            sliderHandleActive = true;
        }
//        else
//        {
//            sliderHandleActive = false;
//        }

    }

    if (sliderHandleActive) {
        slider->sliderHandleRect.x += x;
    }

    return valueChanged;
}

int MUI_GetSliderValue(MUI_Slider *slider) {

}

void MUI_RenderSlider(MUI_Slider *slider, SDL_Renderer *renderer) {
    slider->slidingRect.w = MapFloat(slider->value, slider->minValue, slider->maxValue, slider->borderRect.w, 0);
    slider->sliderHandleRect.x = slider->slidingRect.w + slider->borderRect.x;

    ME_RenderFillRect(renderer, &slider->slidingRect, ME_HexToSdlColor(0xff0000));
    ME_RenderDrawRect(renderer, &slider->borderRect, ME_HexToSdlColor(0xffffff));
    ME_RenderFillRect(renderer, &slider->sliderHandleRect, ME_HexToSdlColor(0x0000ff));

}

void MUI_DestroySlider(MUI_Slider *slider) {

}


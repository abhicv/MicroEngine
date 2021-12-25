#ifndef UI_STYLE_H
#define UI_STYLE_H

#define MUI_ORIGIN_ID 2021

MUI_Style buttonStyle = {
    .buttonStyle = {
        .idleColor = {250, 200, 15},
        .highlightColor = {200, 140, 10},
        .textColor = {0, 0, 0},
        .fontSize = 14,
    },
};

MUI_Style sliderStyle = {
    .sliderStyle = {
        .bgColor = {255, 255, 255},
        .sliderColor = {255, 0, 0},
    }
};

MUI_Style textStyle = {
    .textStyle = {
        .textColor = {255, 255, 255},
    }
};

#endif //UI_STYLE_H

#ifndef _STYLE_H
#define _STYLE_H

#include "..\Engine\src\MicroUI.h"

MUI_Style buttonStyle = {
    .buttonStyle = {
        .idleColor = {100, 100, 200},
        .highlightColor = {50, 50, 100},
        .textColor = {255, 255, 255},
        .fontSize  = 22,
        .enableHighlightBorder = true,
    },
};

MUI_Style entityButtonStyle = {
    .buttonStyle = {
        .idleColor = {18, 150, 0},
        .highlightColor = {12, 100, 0},
        .textColor = {255, 255, 255},
        .fontSize  = 20,
        .enableHighlightBorder = false,
    },
};

MUI_Style textOnlybuttonStyle = {
    .buttonStyle = {
        .idleColor = {18, 18, 18},
        .highlightColor = {30, 30, 30},
        .textColor = {255, 255, 255},
        .fontSize  = 22,
    },
};

MUI_Style textStyle = {
    .textStyle = {
        .textColor = {255, 255, 255},
    }
};

#endif
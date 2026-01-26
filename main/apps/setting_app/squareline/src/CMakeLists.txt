SET(SOURCES components/ui_comp_settingcell.c
    components/ui_comp.c
    screens/ui_Screen1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_right_arrow_png.c
    images/ui_img_wlan_png.c
    images/ui_img_sound_png.c
    images/ui_img_brightness_png.c
    images/ui_img_info_png.c
    fonts/ui_font_DejaVuSans16.c)

add_library(ui ${SOURCES})

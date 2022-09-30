#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/objarray.h"
#include "py/binary.h"

#ifdef EXTMODS_K210_LCDCTL
#include <drv_lcd.h>

static mp_obj_t py_lcdctl_set_direction(mp_obj_t dir_obj)
{
    int dir = mp_obj_get_int(dir_obj);

    lcd_set_direction((lcd_dir_t)dir);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_lcdctl_set_direction_obj, py_lcdctl_set_direction);

STATIC const mp_rom_map_elem_t lcdctl_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR_set_direction), MP_ROM_PTR(&py_lcdctl_set_direction_obj)},
    {MP_ROM_QSTR(MP_QSTR_XY_RLUD), MP_ROM_INT(DIR_XY_RLUD)},
    {MP_ROM_QSTR(MP_QSTR_YX_RLUD), MP_ROM_INT(DIR_YX_RLUD)},
    {MP_ROM_QSTR(MP_QSTR_XY_LRUD), MP_ROM_INT(DIR_XY_LRUD)},
    {MP_ROM_QSTR(MP_QSTR_YX_LRUD), MP_ROM_INT(DIR_YX_LRUD)},
    {MP_ROM_QSTR(MP_QSTR_XY_RLDU), MP_ROM_INT(DIR_XY_RLDU)},
    {MP_ROM_QSTR(MP_QSTR_YX_RLDU), MP_ROM_INT(DIR_YX_RLDU)},
    {MP_ROM_QSTR(MP_QSTR_XY_LRDU), MP_ROM_INT(DIR_XY_LRDU)},
    {MP_ROM_QSTR(MP_QSTR_YX_LRDU), MP_ROM_INT(DIR_YX_LRDU)},

};
STATIC MP_DEFINE_CONST_DICT(k210_lcdctl_dict, lcdctl_locals_dict_table);

const mp_obj_type_t k210_lcdctl_type = {
    {&mp_type_type},
    .name = MP_QSTR_lcdctl,
    .locals_dict = (mp_obj_dict_t *)&k210_lcdctl_dict,
};
#endif

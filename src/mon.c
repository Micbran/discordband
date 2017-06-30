#include "angband.h"

#include "mon.h"
#include <assert.h>

int mon_ac(mon_ptr mon)
{
    mon_race_ptr race = &r_info[mon->r_idx];
    int          ac = race->ac;

    ac += mon->ac_adj;
    ac = ac * mon->mpower / 1000;
    if (ac < 0) ac = 0;
    /* XXX timed buffs */
    return ac;
}

/* Should we show a message to the player for this monster?
 * When monsters battle each other, especially off screen, this
 * can generate a lot of message spam. cf ignore_unview.
 * Messages about monster status should generally be guarded:
 *   if (mon_show_msg(mon))
 *      msg_format("%s is upset!", m_name);
 * However, if the player is the cause, you probably just want:
 *   if (mon->ml)
 *      msg_format("%s resists!", m_name);
 * The reason is that player_can_see_bold() requires illumination,
 * not just telepathy. I guess this distinction is debatable, but
 * the gameplay effects for ranged spells are confusing wrt resistance
 * when mon_show_msg is used. (formerly, this code was is_seen()). */
bool mon_show_msg(mon_ptr mon)
{
    assert(mon);
    if (!mon->ml) return FALSE;
    if (p_ptr->blind) return FALSE;
    if (p_ptr->inside_battle) return TRUE;
    if (!ignore_unview) return TRUE;
    return player_can_see_bold(mon->fy, mon->fx)
        && projectable(py, px, mon->fy, mon->fx);
}

/* mon->r_idx vs mon->ap_r_idx is a bit confusing (to me, anyway)
 * [1] Generally, these are the same.
 * [2] r_idx is what the monster is.
 * [3] ap_r_idx is what the monster appears to be to the player.
 * [4] Tanuki: r_idx = MON_TANUKI. ap_r_idx = current mask. As far as the game
 *     is concerned (monster flags, speed, etc), the monster is a Tanuki. But
 *     the player sees something else (e.g. a Death mold that is moving around!).
 * [5] Chameleon: r_idx = ap_r_idx = current form. Uses MFLAG2_CHAMELEON
 *     to remember that it really is a chameleon. This monster is a shapeshifter
 *     and truly does become different monster forms. You never kill or encounter
 *     a monster with r_idx == MON_CHAMELEON.
 * [6] Shadower: r_idx = true form. ap_r_idx = MON_KAGE (shadower). The shadower's
 *     true nature is hidden from the player until it dies, or is probed.
 *
 * Use mon_race() for gameplay stuff, such as resistances, movement, etc.
 *
 * Use mon_apparent_race() for display stuff, such as monster_desc, the monster
 * list, or monster recall.
 *
 * Use mon_true_race() to recover the original race. This is usually the same
 * as mon_race(), except for chameleons (cf [5] above). Generally, this is done
 * when the monster dies. (real_r_ptr atm).
 *
 * Guard monster lore with is_original_ap (r_idx == ap_r_idx).
 */
mon_race_ptr mon_race(mon_ptr mon)
{
    assert(mon);
    return &r_info[mon->r_idx];
}

mon_race_ptr mon_apparent_race(mon_ptr mon)
{
    assert(mon);
    return &r_info[mon->ap_r_idx];
}

mon_race_ptr mon_true_race(mon_ptr mon)
{
    mon_race_ptr race;
    assert(mon);
    race = &r_info[mon->r_idx];
    if (mon->mflag2 & MFLAG2_CHAMELEON)
    {
        if (race->flags1 & RF1_UNIQUE) /* Chameleon lord only uses unique forms */
            return &r_info[MON_CHAMELEON_K];
        else                           /* other chameleons never use unique forms */
            return &r_info[MON_CHAMELEON];
    }
    return race;
}

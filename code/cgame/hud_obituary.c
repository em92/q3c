#include "../qcommon/q_shared.h"
#include "cg_local.h"
#include "hud_local.h"

#define MAX_OBITUARY 16
#define OBITUARY_TIMEOUT 1500
#define OBITUARY_FADEOUTTIME 1000

typedef struct {
	int	killer;
	int victim;
	weapon_t weapon;
	meansOfDeath_t mod;
	int time;
} obituary_t;

static obituary_t hud_obituary[MAX_OBITUARY];
static int hud_numobituary;

void hud_initobituary( void ) {
	hud_numobituary = 0;
}

static void hud_purgeobituary( void ) {
	static obituary_t obituary[MAX_OBITUARY];
	int i, numobituary;
	memcpy( obituary, hud_obituary, sizeof( obituary ) );
	numobituary = 0;
	for ( i = 0; i < hud_numobituary; i++ ) {
		if ( cg.time - obituary[i].time > OBITUARY_TIMEOUT ) {
			continue;
		}
		memcpy( &hud_obituary[numobituary], &obituary[i], sizeof( obituary_t ) );
		numobituary++;
	}
	hud_numobituary = numobituary;
}

void hud_drawobituary( void ) {
	static float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static float wepcolor[4];
	static float shadow[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	int i;
	float dim, y, x;
	obituary_t *p;
	weapon_t weapon = WP_NONE;
	qhandle_t deathicon;

	if ( cg.showScores || cg.predictedPlayerState.pm_type == PM_DEAD ) {
		return;
	}

	hud_purgeobituary();

	x = 50;
	y = 165;

	for ( p = hud_obituary; p < hud_obituary + hud_numobituary; p++ ) {
		if ( cg.time - p->time > OBITUARY_FADEOUTTIME ) {
			color[3] = 1.0f - ( (float)cg.time - p->time ) / ( OBITUARY_TIMEOUT - OBITUARY_FADEOUTTIME );
		}
		else {
			color[3] = 1.0f;
		}
		shadow[3] = color[3];
		trap_R_SetColor( color );
		if ( p->killer == p->victim || p->killer == ENTITYNUM_WORLD ) {
			hud_drawcolorstring( hud_bounds.left + x, hud_bounds.top + y, 0.4f, hud_media.font_regular, va( "%s ^7%s", cgs.clientinfo[p->victim].name, "suicides" ), shadow, 2, 2, qfalse );
			continue;
		}
		deathicon = hud_media.icon_death;
		if ( p->weapon > WP_NONE && p->weapon < WP_NUM_WEAPONS ) {
			deathicon = hud_media.icon_weapon[p->weapon];
			wepcolor[0] = hud_weapon_colors[p->weapon][0];
			wepcolor[1] = hud_weapon_colors[p->weapon][1];
			wepcolor[2] = hud_weapon_colors[p->weapon][2];
			wepcolor[3] = color[3];
		}
		dim = hud_measurecolorstring( 0.4f, hud_media.font_regular, cgs.clientinfo[p->killer].name );
		hud_drawcolorstring( hud_bounds.left + x, hud_bounds.top + y, 0.4f, hud_media.font_regular, cgs.clientinfo[p->killer].name, shadow, 2, 2, qfalse );
		hud_drawcolorstring( hud_bounds.left + x + dim + 64, hud_bounds.top + y, 0.4f, hud_media.font_regular, cgs.clientinfo[p->victim].name, shadow, 2, 2, qfalse );
		if ( hud_media.icon_death != deathicon ) {
			trap_R_SetColor( wepcolor );
		}
		hud_drawpic( hud_bounds.left + x + dim + 32, hud_bounds.top + y - 8, 48, 48, 0.5f, 0.5f, deathicon );
		y += 40;
	}
	trap_R_SetColor( NULL );
}

void CG_AddObituary( int killer, int victim, weapon_t weapon, meansOfDeath_t mod ) {
	int i;

	if ( hud_numobituary == MAX_OBITUARY ) {
		for ( i = 0; i < MAX_OBITUARY - 1; i++ ) {
			memcpy( &hud_obituary[i], &hud_obituary[i + 1], sizeof( obituary_t ) );
		}
		hud_numobituary--;
	}
	hud_obituary[hud_numobituary].killer = killer;
	hud_obituary[hud_numobituary].victim = victim;
	hud_obituary[hud_numobituary].mod = mod;
	hud_obituary[hud_numobituary].weapon = weapon;
	hud_obituary[hud_numobituary].time = cg.time;
	hud_numobituary++;
}

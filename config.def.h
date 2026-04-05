/* Taken from https://github.com/djpohly/dwl/issues/466 */
#define COLOR(hex)    { ((hex >> 24) & 0xFF) / 255.0f, \
                        ((hex >> 16) & 0xFF) / 255.0f, \
                        ((hex >> 8) & 0xFF) / 255.0f, \
                        (hex & 0xFF) / 255.0f }
/* appearance */
static const int sloppyfocus               = 0;  /* focus follows mouse */
static const int bypass_surface_visibility = 0;  /* 1 means idle inhibitors will disable idle tracking even if it's surface isn't visible  */
static const int smartgaps                 = 0;  /* 1 means no outer gap when there is only one window */
static int gaps                            = 1;  /* 1 means gaps between windows are added */
static const unsigned int gappx            = 8; /* gap pixel between windows */
static const unsigned int borderpx         = 2;  /* border pixel of windows */
static const int showbar                   = 1; /* 0 means no bar */
static const int topbar                    = 0; /* 0 means bottom bar */
static const char *fonts[]                 = {"monospace:size=13"};
static const float rootcolor[]             = COLOR(0x000000ff);
/* This conforms to the xdg-protocol. Set the alpha to zero to restore the old behavior */
static const float fullscreen_bg[]         = {0.0f, 0.0f, 0.0f, 1.0f}; /* You can also use glsl colors */
static uint32_t colors[][3]                = {
	/*               fg          bg          border    */
	[SchemeNorm] = { 0xbbbbbbff, 0x222222ff, 0x444444ff },
	[SchemeSel]  = { 0xeeeeeeff, 0x005577ff, 0x005577ff },
	[SchemeUrg]  = { 0,          0,          0x770000ff },
};

/* tagging */
static char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

enum {
	floating,
	wlrctl,
	passthrough,
};

const char *modes_labels[] = {
	"floating",
    	"[hjkl]cursors [,]left [.]right [m]iddle ^[hjkl]arrows",
    	"passthrough",
};

/* logging */
static int log_level = WLR_ERROR;

static const Rule rules[] = {
	/* app_id             title       tags mask  isfloating monitor */
	{ NULL,         "Floating_Term", 	0,	1,	-1 },
	{ "file-*",           NULL,       	0,	1,	-1 },
	{ NULL,		"Open Folder",       	0,	1,	-1 },
	{ NULL,		"Open File",       	0,	1,	-1 },
	{ "org.fcitx.fcitx5-config-qt", NULL, 	0,	1,	-1 },
};

/* layout(s) */
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* monitors */
/* (x=-1, y=-1) is reserved as an "autoconfigure" monitor position indicator
 * WARNING: negative values other than (-1, -1) cause problems with Xwayland clients due to
 * https://gitlab.freedesktop.org/xorg/xserver/-/issues/899 */
static const MonitorRule monrules[] = {
   /* name        mfact  nmaster scale layout       rotate/reflect                x    y
    * example of a HiDPI laptop monitor:
    { "eDP-1",    0.5f,  1,      2,    &layouts[0], WL_OUTPUT_TRANSFORM_NORMAL,   -1,  -1 }, */
	{ NULL,       0.55f, 1,      1,    &layouts[0], WL_OUTPUT_TRANSFORM_NORMAL,   -1,  -1 },
	/* default monitor rule: can be changed but cannot be eliminated; at least one monitor rule must exist */
};

/* keyboard */
static const struct xkb_rule_names xkb_rules = {
	/* can specify fields: rules, model, layout, variant, options */
	/* example:
	.options = "ctrl:nocaps",
	*/
	.options = NULL,
};

static const int repeat_rate = 80;
static const int repeat_delay = 300;

/* Trackpad */
static const int tap_to_click = 1;
static const int tap_and_drag = 1;
static const int drag_lock = 1;
static const int natural_scrolling = 0;
static const int disable_while_typing = 1;
static const int left_handed = 0;
static const int middle_button_emulation = 0;
/* You can choose between:
LIBINPUT_CONFIG_SCROLL_NO_SCROLL
LIBINPUT_CONFIG_SCROLL_2FG
LIBINPUT_CONFIG_SCROLL_EDGE
LIBINPUT_CONFIG_SCROLL_ON_BUTTON_DOWN
*/
static const enum libinput_config_scroll_method scroll_method = LIBINPUT_CONFIG_SCROLL_2FG;

/* You can choose between:
LIBINPUT_CONFIG_CLICK_METHOD_NONE
LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS
LIBINPUT_CONFIG_CLICK_METHOD_CLICKFINGER
*/
static const enum libinput_config_click_method click_method = LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS;

/* You can choose between:
LIBINPUT_CONFIG_SEND_EVENTS_ENABLED
LIBINPUT_CONFIG_SEND_EVENTS_DISABLED
LIBINPUT_CONFIG_SEND_EVENTS_DISABLED_ON_EXTERNAL_MOUSE
*/
static const uint32_t send_events_mode = LIBINPUT_CONFIG_SEND_EVENTS_ENABLED;

/* You can choose between:
LIBINPUT_CONFIG_ACCEL_PROFILE_FLAT
LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE
*/
static const enum libinput_config_accel_profile accel_profile = LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE;
static const double accel_speed = 0.0;

/* You can choose between:
LIBINPUT_CONFIG_TAP_MAP_LRM -- 1/2/3 finger tap maps to left/right/middle
LIBINPUT_CONFIG_TAP_MAP_LMR -- 1/2/3 finger tap maps to left/middle/right
*/
static const enum libinput_config_tap_button_map button_map = LIBINPUT_CONFIG_TAP_MAP_LRM;

static const int hide_cursor_when_typing = 1;

static const int cursor_timeout = 3;

/* If you want to use the windows key for MODKEY, use WLR_MODIFIER_LOGO */
#define MODKEY WLR_MODIFIER_LOGO

#define TAGKEYS(KEY,SKEY,TAG) \
	{ MODKEY,                    KEY,            view,            {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_CTRL,  KEY,            toggleview,      {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_SHIFT, SKEY,           tag,             {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_CTRL|WLR_MODIFIER_SHIFT,SKEY,toggletag, {.ui = 1 << TAG} }

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static const char *termcmd[] = { "foot", NULL };
static const char *menucmd[] = { "wmenu-run -i", NULL };

static const Key keys[] = {
	/* Note that Shift changes certain key codes: 2 -> at, etc. */
	/* modifier                  key                  function          argument */
	{ MODKEY,                    XKB_KEY_p,           spawn,            {.v = menucmd} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_Return,      spawn,            {.v = termcmd} },
	{ MODKEY,                    XKB_KEY_b,           togglebar,        {0} },
	{ MODKEY,                    XKB_KEY_j,           focusstack,       {.i = +1} },
	{ MODKEY,                    XKB_KEY_k,           focusstack,       {.i = -1} },
	{ MODKEY|WLR_MODIFIER_CTRL,  XKB_KEY_h,        	  focusdir,         {.ui = 0} },
	{ MODKEY|WLR_MODIFIER_CTRL,  XKB_KEY_l,       	  focusdir,         {.ui = 1} },
	{ MODKEY|WLR_MODIFIER_CTRL,  XKB_KEY_k,        	  focusdir,         {.ui = 2} },
	{ MODKEY|WLR_MODIFIER_CTRL,  XKB_KEY_j,        	  focusdir,         {.ui = 3} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_h,        	  swapdir,          {.ui = 0} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_l,       	  swapdir,          {.ui = 1} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_k,        	  swapdir,          {.ui = 2} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_j,        	  swapdir,          {.ui = 3} },
	{ MODKEY,                    XKB_KEY_i,           incnmaster,       {.i = +1} },
	{ MODKEY,                    XKB_KEY_d,           incnmaster,       {.i = -1} },
	{ MODKEY,                    XKB_KEY_h,           setmfact,         {.f = -0.05f} },
	{ MODKEY,                    XKB_KEY_l,           setmfact,         {.f = +0.05f} },
	{ MODKEY,                    XKB_KEY_Return,      zoom,             {0} },
	{ MODKEY,                    XKB_KEY_Tab,         view,             {0} },
	{ MODKEY,                    XKB_KEY_g,           togglegaps,       {0} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_c,           killclient,       {0} },
	{ MODKEY,                    XKB_KEY_t,           setlayout,        {.v = &layouts[0]} },
	{ MODKEY,                    XKB_KEY_f,           setlayout,        {.v = &layouts[1]} },
	{ MODKEY,                    XKB_KEY_m,           setlayout,        {.v = &layouts[2]} },
	{ MODKEY,                    XKB_KEY_space,       setlayout,        {0} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_space,       togglefloating,   {0} },
	{ MODKEY,                    XKB_KEY_e,           togglefullscreen, {0} },
	{ MODKEY,                    XKB_KEY_0,           view,             {.ui = ~0} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_parenright,  tag,              {.ui = ~0} },
	{ MODKEY,                    XKB_KEY_comma,       focusmon,         {.i = WLR_DIRECTION_LEFT} },
	{ MODKEY,                    XKB_KEY_period,      focusmon,         {.i = WLR_DIRECTION_RIGHT} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_less,        tagmon,           {.i = WLR_DIRECTION_LEFT} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_greater,     tagmon,           {.i = WLR_DIRECTION_RIGHT} },
	TAGKEYS(          XKB_KEY_1, XKB_KEY_exclam,                        0),
	TAGKEYS(          XKB_KEY_2, XKB_KEY_at,                            1),
	TAGKEYS(          XKB_KEY_3, XKB_KEY_numbersign,                    2),
	TAGKEYS(          XKB_KEY_4, XKB_KEY_dollar,                        3),
	TAGKEYS(          XKB_KEY_5, XKB_KEY_percent,                       4),
	TAGKEYS(          XKB_KEY_6, XKB_KEY_asciicircum,                   5),
	TAGKEYS(          XKB_KEY_7, XKB_KEY_ampersand,                     6),
	TAGKEYS(          XKB_KEY_8, XKB_KEY_asterisk,                      7),
	TAGKEYS(          XKB_KEY_9, XKB_KEY_parenleft,                     8),
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_q,           quit,             {0} },

	{ MODKEY,                    XKB_KEY_x,           spawn,            SHCMD("$BROWSER") },

	{ MODKEY,                    XKB_KEY_minus,       spawn,            SHCMD("${HOME}/.local/bin/vol 2%-") },
	{ MODKEY,                    XKB_KEY_equal,       spawn,            SHCMD("${HOME}/.local/bin/vol 2%+") },
	{ MODKEY,                    XKB_KEY_BackSpace,   spawn,            SHCMD("${HOME}/.local/bin/vol mute") },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_minus,       spawn,            SHCMD("${HOME}/.local/bin/vol -m 2%-") },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_equal,       spawn,            SHCMD("${HOME}/.local/bin/vol -m 2%+") },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_BackSpace,   spawn,            SHCMD("${HOME}/.local/bin/vol -m mute") },

	{ MODKEY,                    XKB_KEY_bracketleft, spawn,            SHCMD("${HOME}/.local/bin/bl 2%-") },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_braceleft,   spawn,            SHCMD("${HOME}/.local/bin/bl -e 2%-") },
	{ MODKEY,                    XKB_KEY_bracketright,spawn,            SHCMD("${HOME}/.local/bin/bl 2%+") },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_braceright,  spawn,            SHCMD("${HOME}/.local/bin/bl -e 2%+") },

	{ MODKEY|WLR_MODIFIER_SHIFT|WLR_MODIFIER_CTRL, 	XKB_KEY_L,  spawn,  SHCMD("waylock -ignore-empty-password -init-color 0x000000 -input-color 0x005577 -fail-color 0xcc3333") },

	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_N,           spawn,            SHCMD("fnottctl dismiss") },

	{ MODKEY,                    XKB_KEY_y,           spawn,            SHCMD("${HOME}/.local/bin/shot") },
	{ MODKEY|WLR_MODIFIER_ALT,   XKB_KEY_y,           spawn,            SHCMD("${HOME}/.local/bin/shot --window") },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_Y,           spawn,            SHCMD("${HOME}/.local/bin/shot --geo") },
	{ MODKEY|WLR_MODIFIER_CTRL,  XKB_KEY_y,           spawn,            SHCMD("${HOME}/.local/bin/shot --all") },
	{ MODKEY|WLR_MODIFIER_SHIFT|WLR_MODIFIER_CTRL,  XKB_KEY_y,  spawn,  SHCMD("${HOME}/.local/bin/shot --show") },
	{ MODKEY|WLR_MODIFIER_SHIFT|WLR_MODIFIER_CTRL,  XKB_KEY_w,  spawn,  SHCMD("${HOME}/.local/bin/rec") },

	{ MODKEY,                    XKB_KEY_c,           spawn,            SHCMD("${HOME}/.local/bin/clip") },
	{ MODKEY|WLR_MODIFIER_CTRL,  XKB_KEY_c,           spawn,            SHCMD("cliphist wipe") },

	{ MODKEY,                    XKB_KEY_a,           spawn,            SHCMD("${HOME}/.local/bin/bm") },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_A,           spawn,            SHCMD("${HOME}/.local/bin/bm -a") },

	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_B,           spawn,            SHCMD("${HOME}/.local/bin/bt") },
	{ MODKEY|WLR_MODIFIER_CTRL,  XKB_KEY_B,		  spawn,            SHCMD("${HOME}/.local/bin/ef") },

	{ MODKEY|WLR_MODIFIER_CTRL,  XKB_KEY_p,		  spawn,            SHCMD("mpc toggle") },
	{ MODKEY|WLR_MODIFIER_CTRL,  XKB_KEY_bracketleft, spawn,            SHCMD("mpc prev") },
	{ MODKEY|WLR_MODIFIER_CTRL,  XKB_KEY_bracketright,spawn,            SHCMD("mpc next") },

	{ MODKEY|WLR_MODIFIER_CTRL,  XKB_KEY_w,           spawn,            SHCMD("${HOME}/.local/bin/passmenu --type") },

	{ MODKEY|WLR_MODIFIER_SHIFT|WLR_MODIFIER_CTRL,  XKB_KEY_f,  spawn,  SHCMD("${HOME}/.local/bin/fztrans") },
	{ MODKEY|WLR_MODIFIER_CTRL,  XKB_KEY_t,        	  spawn,            SHCMD("footclient -T Floating_Term ${HOME}/.local/bin/fzkill") },

	{ MODKEY|WLR_MODIFIER_CTRL,  XKB_KEY_f,           entermode,        {.i = floating} },
	{ MODKEY,                    XKB_KEY_slash,       entermode,        {.i = wlrctl} },
	{ MODKEY,		     XKB_KEY_Escape,      entermode,        {.i = passthrough} },

	/* Ctrl-Alt-Backspace and Ctrl-Alt-Fx used to be handled by X server */
	{ WLR_MODIFIER_CTRL|WLR_MODIFIER_ALT,XKB_KEY_Terminate_Server, quit, {0} },
	/* Ctrl-Alt-Fx is used to switch to another VT, if you don't know what a VT is
	 * do not remove them.
	 */
#define CHVT(n) { WLR_MODIFIER_CTRL|WLR_MODIFIER_ALT,XKB_KEY_XF86Switch_VT_##n, chvt, {.ui = (n)} }
	CHVT(1), CHVT(2), CHVT(3), CHVT(4), CHVT(5), CHVT(6),
	CHVT(7), CHVT(8), CHVT(9), CHVT(10), CHVT(11), CHVT(12),
};

static const Modekey modekeys[] = {
	/* mode   modifier              key                 function    	argument */
	{ floating, { 0,                XKB_KEY_Escape,     entermode,		{.i = NORMAL} } },
	{ floating, { 0,                XKB_KEY_space,      entermode,  	{.i = NORMAL} } },
	{ floating, { MODKEY|WLR_MODIFIER_CTRL, XKB_KEY_f,  entermode,  	{.i = NORMAL} } },
	{ floating, { 0,                XKB_KEY_h,          moveresizekb, 	{.v = (int []){ -50, 0, 0, 0 } } } },
	{ floating, { 0,                XKB_KEY_j,          moveresizekb, 	{.v = (int []){ 0, 50, 0, 0 } } } },
	{ floating, { 0,                XKB_KEY_k,          moveresizekb, 	{.v = (int []){ 0, -50, 0, 0 } } } },
	{ floating, { 0,                XKB_KEY_l,          moveresizekb, 	{.v = (int []){ 50, 0, 0, 0 } } } },
	{ floating, { 0,                XKB_KEY_y,          moveresizekb, 	{.v = (int []){ 0, 0, -50, 0 } } } },
	{ floating, { 0,                XKB_KEY_u,          moveresizekb, 	{.v = (int []){ 0, 0, 0, 50 } } } },
	{ floating, { 0,                XKB_KEY_i,          moveresizekb, 	{.v = (int []){ 0, 0, 0, -50 } } } },
	{ floating, { 0,                XKB_KEY_o,          moveresizekb, 	{.v = (int []){ 0, 0, 50, 0 } } } },
	{ wlrctl, { 0,                  XKB_KEY_Escape,     entermode,  	{.i = NORMAL} } },
	{ wlrctl, { 0,                  XKB_KEY_space,      entermode,  	{.i = NORMAL} } },
	{ wlrctl, { MODKEY,             XKB_KEY_slash,      entermode,  	{.i = NORMAL} } },
	{ wlrctl, { 0,                  XKB_KEY_h,          spawn,      	SHCMD("wlrctl pointer move -90 0") } },
	{ wlrctl, { 0,                  XKB_KEY_j,          spawn,      	SHCMD("wlrctl pointer move 0 90") } },
	{ wlrctl, { 0,                  XKB_KEY_k,          spawn,      	SHCMD("wlrctl pointer move 0 -90") } },
	{ wlrctl, { 0,                  XKB_KEY_l,          spawn,      	SHCMD("wlrctl pointer move 90 0") } },
	{ wlrctl, { WLR_MODIFIER_SHIFT, XKB_KEY_H,          spawn,      	SHCMD("wlrctl pointer move -15 0") } },
	{ wlrctl, { WLR_MODIFIER_SHIFT, XKB_KEY_J,          spawn,      	SHCMD("wlrctl pointer move 0 15") } },
	{ wlrctl, { WLR_MODIFIER_SHIFT, XKB_KEY_K,          spawn,      	SHCMD("wlrctl pointer move 0 -15") } },
	{ wlrctl, { WLR_MODIFIER_SHIFT, XKB_KEY_L,          spawn,      	SHCMD("wlrctl pointer move 15 0") } },
	{ wlrctl, { 0,                  XKB_KEY_comma,      spawn,      	SHCMD("wlrctl pointer click left") } },
	{ wlrctl, { 0,                  XKB_KEY_period,     spawn,      	SHCMD("wlrctl pointer click right") } },
	{ wlrctl, { WLR_MODIFIER_SHIFT, XKB_KEY_less,       spawn,      	SHCMD("wlrctl pointer click left") } },
	{ wlrctl, { WLR_MODIFIER_SHIFT, XKB_KEY_greater,    spawn,      	SHCMD("wlrctl pointer click right") } },
	{ wlrctl, { WLR_MODIFIER_CTRL,  XKB_KEY_h,          spawn,      	SHCMD("wtype -k Left") } },
	{ wlrctl, { WLR_MODIFIER_CTRL,  XKB_KEY_j,          spawn,      	SHCMD("wtype -k Down") } },
	{ wlrctl, { WLR_MODIFIER_CTRL,  XKB_KEY_k,          spawn,      	SHCMD("wtype -k Up") } },
	{ wlrctl, { WLR_MODIFIER_CTRL,  XKB_KEY_l,          spawn,      	SHCMD("wtype -k Right") } },
	{ wlrctl, { 0,                  XKB_KEY_n,          spawn,      	SHCMD("wlrctl pointer scroll 30") } },
	{ wlrctl, { 0,                  XKB_KEY_p,          spawn,      	SHCMD("wlrctl pointer scroll -30") } },
	{ passthrough, { MODKEY|WLR_MODIFIER_SHIFT,         XKB_KEY_Escape,	entermode,  {.i = NORMAL} } },
};

static const Button buttons[] = {
	{ ClkLtSymbol, 0,      BTN_LEFT,   setlayout,      {.v = &layouts[0]} },
	{ ClkLtSymbol, 0,      BTN_RIGHT,  setlayout,      {.v = &layouts[2]} },
	{ ClkTitle,    0,      BTN_MIDDLE, zoom,           {0} },
	{ ClkStatus,   0,      BTN_MIDDLE, spawn,          {.v = termcmd} },
	{ ClkClient,   MODKEY, BTN_LEFT,   moveresize,     {.ui = CurMove} },
	{ ClkClient,   MODKEY, BTN_MIDDLE, togglefloating, {0} },
	{ ClkClient,   MODKEY, BTN_RIGHT,  moveresize,     {.ui = CurResize} },
	{ ClkTagBar,   0,      BTN_LEFT,   view,           {0} },
	{ ClkTagBar,   0,      BTN_RIGHT,  toggleview,     {0} },
	{ ClkTagBar,   MODKEY, BTN_LEFT,   tag,            {0} },
	{ ClkTagBar,   MODKEY, BTN_RIGHT,  toggletag,      {0} },
};

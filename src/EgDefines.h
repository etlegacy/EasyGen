#ifndef EGDEFINES_H
#define EGDEFINES_H
// FILENAME WORKSPACE APPTITLE
#define WTITLE_FORMAT       "%s - [%s\\] - %s"

#define GRID_MAX_SIZE       1025
#define GRID_MAX_CELLS      1024
#define GRID_MIN_WIDTH      64

#define GRID_Z_BUFFERS      2
#define GRID_Z_REAL         0
#define GRID_Z_PREVIEW      1

#define GRID_COL_UP         0
#define GRID_COL_MID        1
#define GRID_COL_DN         2

#define GRID_COLORS3        6

#define MODIFIERS       3
#define MOD_NONE        0
#define MOD_SMOOTH      1
#define MOD_BASE        2

#define MOD_MODE_ADD        0
#define MOD_MODE_MAX        1
#define MOD_MODE_IDENT      2

//custom modifier flags
#define MOD_FLAG_ADD                0x00000001
#define MOD_FLAG_MAX                0x00000002
#define MOD_FLAG_IDENT              0x00000004
#define MOD_FLAG_RADIUS             0x00000008
#define MOD_FLAG_SQUARERADIUS       0x00000010
#define MOD_FLAG_POSITIVEHEIGHT     0x00000020

#define SLIDER_NOISE_MAX    100
#define SLIDER_SMOOTH_MAX   10
#define SLIDER_SCALE_MAX    200

#define MAX_STRENGTH    10
#define MAX_RADIUS      10

//#define RENDERMODES			5
#define R_WIREFRAME         0
#define R_FLAT              1
#define R_TEXTURE           2
#define R_ALPHAMAP          3

#define R_FL_MASK           0xF0000000
#define R_FL_QUADFRAME      0x10000000
#define R_FL_TRISFRAME      0x20000000


#define FLAT_SHADE          0
#define FLAT_SMOOTH         1

// face flags (tris orientation...)
#define FACE_FL_TRISINV     0x00000001

// status bar panes
#define STATUS_CMD          0
#define STATUS_MODIFIER     1
#define STATUS_PLAYER       2   // should be STATUS_POS...
#define STATUS_GRID         3

// from view tabs
#define TAB_GRID            0
#define TAB_MODIFIER        1
#define TAB_ALPHAMAP        2
#define TAB_MODELS          3

#define ALIGN_RIGHT         0
#define ALIGN_TOP           0
#define ALIGN_CENTER        1
#define ALIGN_MIDDLE        1
#define ALIGN_LEFT          2
#define ALIGN_BOTTOM        2

#define START_CELLSX        12
#define START_CELLSY        12
#define START_WIDTHX        3072
#define START_WIDTHY        3072

#define PAL_WIDTH           160
#define SELCOL_WIDTH        64
#define SELTEX_WIDTH        64

//Models mini-viewport (square)
#define VIEWXZ_WIDTH        96
#define VIEWYZ_WIDTH        96

//FlipDlgModes
#define FLIP_A              0
#define FLIP_B              1
#define FLIP_C              2
#define FLIP_D              3
#define FLIP_E              4
#define FLIP_F              5
#define FLIP_G              6
#define FLIP_H              7

//metashaders
#define MS_CLOUDS           0x00000001

#define SLEEP_TIME          200

#define BLACK               RGB(0, 0, 0)

#define ASSUME_ZERO         0.4f

#define PI                  3.1415926535897932384626433832795f

#endif
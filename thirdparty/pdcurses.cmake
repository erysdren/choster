## ========================================================
##
## FILE:			/thirdparty/pdcurses.cmake
##
## AUTHORS:			Jaycie Ewald
##
## PROJECT:			libcohost
##
## LICENSE:			ACSL v1.4
##
## DESCRIPTION:		PDCurses CMake project definition
##
## LAST EDITED:		November 26th, 2022
##
## ========================================================

##
## Definitions
##

## Project
project(PDCurses LANGUAGES C)

##
## Libraries
##

set(PDCURSES_SRC
	${PDCURSES_DIR}/pdcurses/addch.c
	${PDCURSES_DIR}/pdcurses/addchstr.c
	${PDCURSES_DIR}/pdcurses/addstr.c
	${PDCURSES_DIR}/pdcurses/attr.c
	${PDCURSES_DIR}/pdcurses/beep.c
	${PDCURSES_DIR}/pdcurses/bkgd.c
	${PDCURSES_DIR}/pdcurses/border.c
	${PDCURSES_DIR}/pdcurses/clear.c
	${PDCURSES_DIR}/pdcurses/color.c
	${PDCURSES_DIR}/pdcurses/delch.c
	${PDCURSES_DIR}/pdcurses/deleteln.c
	${PDCURSES_DIR}/pdcurses/getch.c
	${PDCURSES_DIR}/pdcurses/getstr.c
	${PDCURSES_DIR}/pdcurses/getyx.c
	${PDCURSES_DIR}/pdcurses/inch.c
	${PDCURSES_DIR}/pdcurses/inchstr.c
	${PDCURSES_DIR}/pdcurses/initscr.c
	${PDCURSES_DIR}/pdcurses/inopts.c
	${PDCURSES_DIR}/pdcurses/insch.c
	${PDCURSES_DIR}/pdcurses/insstr.c
	${PDCURSES_DIR}/pdcurses/instr.c
	${PDCURSES_DIR}/pdcurses/kernel.c
	${PDCURSES_DIR}/pdcurses/keyname.c
	${PDCURSES_DIR}/pdcurses/mouse.c
	${PDCURSES_DIR}/pdcurses/move.c
	${PDCURSES_DIR}/pdcurses/outopts.c
	${PDCURSES_DIR}/pdcurses/overlay.c
	${PDCURSES_DIR}/pdcurses/pad.c
	${PDCURSES_DIR}/pdcurses/panel.c
	${PDCURSES_DIR}/pdcurses/printw.c
	${PDCURSES_DIR}/pdcurses/refresh.c
	${PDCURSES_DIR}/pdcurses/scanw.c
	${PDCURSES_DIR}/pdcurses/scr_dump.c
	${PDCURSES_DIR}/pdcurses/scroll.c
	${PDCURSES_DIR}/pdcurses/slk.c
	${PDCURSES_DIR}/pdcurses/termattr.c
	${PDCURSES_DIR}/pdcurses/touch.c
	${PDCURSES_DIR}/pdcurses/util.c
	${PDCURSES_DIR}/pdcurses/window.c
	${PDCURSES_DIR}/pdcurses/debug.c
)

set(PDCURSES_DOS_SRC
	${PDCURSES_DIR}/dos/pdcclip.c
	${PDCURSES_DIR}/dos/pdcdisp.c
	${PDCURSES_DIR}/dos/pdcgetsc.c
	${PDCURSES_DIR}/dos/pdckbd.c
	${PDCURSES_DIR}/dos/pdcscrn.c
	${PDCURSES_DIR}/dos/pdcsetsc.c
	${PDCURSES_DIR}/dos/pdcutil.c
)

add_library(pdcurses STATIC ${PDCURSES_SRC} ${PDCURSES_DOS_SRC})
target_include_directories(pdcurses PUBLIC ${PDCURSES_DIR})

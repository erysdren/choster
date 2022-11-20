// ========================================================
//
// FILE:			/utils.c
//
// AUTHORS:			Jaycie Ewald
//
// PROJECT:			libcohost
//
// LICENSE:			ACSL v1.4
//
// DESCRIPTION:		Random utilities
//
// LAST EDITED:		November 20th, 2022
//
// ========================================================

#include "cohost.h"

char *replace_char(char *string, char find, char replace)
{
	char *current_pos = strchr(string, find);
 
	while (current_pos)
	{
		*current_pos = replace;
		current_pos = strchr(current_pos + 1, find);
	}

	return string;
}

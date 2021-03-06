/*
 *  Text-based dungeon/adventure game, for the
 *  UW Experimental College course in Intermediate C Programming
 *
 *  Copyright 1996-1999 Steve Summit
 *  scs@eskimo.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "defs.h"

static dircommand(struct actor *, int);


docommand(struct actor *player, struct sentence *cmd)
{
char * ch; 
char *verb;
struct object *objp;

verb = cmd->verb;
objp = cmd->object;


if(strcmp(verb, "n") == 0 || strcmp(verb, "north") == 0)
	dircommand(player, NORTH);
else if(strcmp(verb, "s") == 0 || strcmp(verb, "south") == 0)
	dircommand(player, SOUTH);
else if(strcmp(verb, "e") == 0 || strcmp(verb, "east") == 0)
	dircommand(player, EAST);
else if(strcmp(verb, "w") == 0 || strcmp(verb, "west") == 0)
	dircommand(player, WEST);
else if(strcmp(verb, "u") == 0 || strcmp(verb, "up") == 0)
	dircommand(player, UP); 
else if(strcmp(verb, "d") == 0 || strcmp(verb, "down") == 0)
	dircommand(player, DOWN); 
else if(strcmp(verb, "examine") == 0 || strcmp(verb, "ex") == 0)
	{
	if(objp == NULL)
		{
		printf("You must tell me what to examine.\n");
		return FALSE;
		}
	if(contains(player->location->contents, objp)
		||contains(player->contents, objp))
		{
		if(objp->desc==NULL && objp->attrs==NULL)
			printf("Nothing special about this object.\n");
		if(objp->desc!=NULL)
			{
			ch = getLast(objp->name); 
			printf("The %s %s %s.\n", objp, 
			*ch=='s'?"are":"is" ,objp->desc); 
			}
		if(objp->attrs!=0) // attrs is int
			{
			printf("%s attributes: ", objp->name); 
			if(objp->attrs & LOCKED)	printf("locked "); 
			if(objp->attrs & CLOSABLE)	printf("closable "); 
			if(objp->attrs & OPEN)	printf("open "); 
			if(objp->attrs & HEAVY)	printf("heavy "); 
			if(objp->attrs & BROKEN)	printf("broken "); 
			if(objp->attrs & SOFT)	printf("soft "); 
			if(objp->attrs & SHARP)	printf("sharp "); 
			if(objp->attrs & TRANSPARENT)	printf("transparent "); 
			if(objp->attrs & IMMOBILE)	printf("immobile"); 
			if(objp->attrs & CONTAINER)	printf("container "); 
			if(objp->attrs & TOOL)	printf("tool "); 
			if(objp->attrs & SAFE)	printf("safe "); //was lock
			if(objp->attrs & KEY)	printf("key "); 
			printf("\n");
			}
		return FALSE;
		}
	}
else if(strcmp(verb, "t") == 0 || strcmp(verb, "take") == 0)
	{
	if(objp == NULL)
		{
		printf("You must tell me what to take.\n");
		return FALSE;
		}
	if(contains(player->contents, objp))
		{
		printf("You already have the %s.\n", objp->name);
		return FALSE;
		}
	if(!takeobject(player, objp))
		{
		/* shouldn't happen */
		printf("You can't pick up the %s.\n", objp->name);
		return FALSE;
		}
	printf("Taken.\n");
	}
else if(strcmp(verb, "drop") == 0)
	{
	if(objp == NULL)
		{
		printf("You must tell me what to drop.\n");
		return FALSE;
		}
	if(!dropobject(player, objp))
		{
		/* shouldn't happen */
		printf("You can't drop the %s!\n", objp->name);
		return FALSE;
		}
	printf("Dropped.\n");
	}
else if(strcmp(verb, "l") == 0 || strcmp(verb, "look") == 0) 
	{
		/*room contents*/
	if(player->location->contents != NULL)
		{
		printf("room contains: ");
		listobjects(player->location->contents);
		printf("\n");
		}
		/*room description*/
	if(player->location->desc != NULL)
		{
		printf("%s\n", player->location->desc);
		}
	}
else if(strcmp(verb, "i") == 0 || strcmp(verb, "inventory") == 0)
	{
	if(player->contents == NULL)
		printf("You are empty handed.\n");
	else	{
		printf("You are carrying: ");
		listobjects(player->contents);
		printf("\n");
		}
	}
else if(strcmp(verb, "break") == 0)
	{
	if(objp == NULL)
		{
		printf("You must tell me what to hit.\n");
		return FALSE;
		}
	if(cmd->preposition == NULL || strcmp(cmd->preposition, "with") != 0 ||
			cmd->xobject == NULL)
		{
		printf("You must tell me what to break with.\n");
		return FALSE;
		}
	if(!(cmd->xobject->attrs & HEAVY))
		{
		ch = getLast(cmd->xobject->name); //+
		printf("I don't think the %s %s heavy enough to break things with.\n",
		cmd->xobject->name, 	*ch == 's'? "are" : "is" );
		return FALSE;
		}
	objp->attrs |= BROKEN;
	ch = getLast(objp->name);  //+
	printf("The %s %s broken\n", objp->name,
		*ch == 's'? "is" : "are" );  
	}
else if(strcmp(verb, "cut") == 0)
	{
	if(objp == NULL)
		{
		printf("You must tell me what to cut.\n");
		return FALSE;
		}
	if(cmd->preposition == NULL || strcmp(cmd->preposition, "with") != 0 ||
			cmd->xobject == NULL)
		{
		printf("You must tell me what to cut with.\n");
		return FALSE;
		}
	if(!(cmd->xobject->attrs & SHARP))
		{
		ch = getLast(cmd->xobject->name); //+
		printf("I don't think the %s %s sharp enough to cut things with.\n",
		cmd->xobject->name,*ch == 's'? "are" : "is" );
		return FALSE;
		}
		if(!(objp->attrs & SOFT))
			{
			printf("I don't think you can cut the %s with the %s.\n",
			objp->name, cmd->xobject->name);
			return FALSE;
			}
		ch = getLast(objp->name);
		printf("The %s %s now cut in two.\n", objp->name,
			*ch == 's'? "is" : "are");
		}
//+ Week4 commands: open, close, put
/////////////////////////////////////
else if(strcmp(verb, "open") == 0)
	{
	if(objp == NULL)
		{
		printf("You must tell me what to open.\n");
		return FALSE;
		}
	if(Isopen(objp))
		{
		printf("The %s is already open.\n", objp->name);
		return FALSE;
		}
	if(!(objp->attrs & CLOSABLE))
		{
		printf("You can't open the %s.\n", objp->name);
		return FALSE;
		}
	objp->attrs |= OPEN;
	printf("The %s is now open.\n", objp->name);
	}

else if(strcmp(verb, "close") == 0)
	{
	if(objp == NULL)
		{
		printf("You must tell me what to close.\n");
		return FALSE;
		}
	if(!(objp->attrs & CLOSABLE))
		{
		printf("You can't close the %s.\n", objp->name);
		return FALSE;
		}
	if(!Isopen(objp))
		{
		printf("The %s is already closed.\n", objp->name);
		return FALSE;
		}
	objp->attrs &= ~OPEN;
	printf("The %s is now closed.\n", objp->name);
	}

else if(strcmp(verb, "put") == 0)
	{
	if(objp == NULL)
		{
		printf("You must tell me what to put.\n");
		return FALSE;
		}
	if(!contains(player->contents, objp))
		{
		printf("You don't have the %s.\n", objp->name);
		return FALSE;
		}
	if(cmd->preposition == NULL || strcmp(cmd->preposition, "in") != 0 ||
			cmd->xobject == NULL)
		{
		printf("You must tell me where to put the %s.\n",
							objp->name);
		return FALSE;
		}
	if(!Iscontainer(cmd->xobject))
		{
		printf("You can't put things in the %s.\n",
							cmd->xobject->name);
		return FALSE;
		}
	if((cmd->xobject->attrs & CLOSABLE) && !Isopen(cmd->xobject))
		{
		printf("The %s is closed.\n", cmd->xobject->name);
		return FALSE;
		}
	if(!putobject(player, objp, cmd->xobject))
		{
		/* shouldn't happen */
		printf("You can't put the %s in the %s!\n",
			objp->name, cmd->xobject->name);
		return FALSE;
		}
	printf("Now the %s is in the %s.\n",
			objp->name, cmd->xobject->name);
	}
////////////////////////////////

else if(strcmp(verb, "quit") == 0)
	{
	exit(0);
	}
else	{
	printf("I don't know the word \"%s\".\n", verb);
	return FALSE;
	}
return TRUE;
}

/* Common code for the n, e, w, and s commands.			*/
/* This function just picks the correct exit link (within the	*/
/* room structure) to act on; the gotoroom() function		*/
/* (in rooms.c) does the real work.				*/

dircommand(struct actor *player, int dir)
{
struct room *roomp = player->location;

if(roomp == NULL)
	{
	printf("Where are you?\n");
	return FALSE;
	}

/* If the exit does not exist, or if gotoroom() fails,	*/
/* the player can't go that way.			*/
if(roomp->exits[dir] == NULL || !gotoroom(player, roomp->exits[dir]))
	{
	printf("You can't go that way.\n");
	return FALSE;
	}

/* player now in new room */

listroom(player);

return TRUE;
}

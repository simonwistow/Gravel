/*
 * Copyright (C) 2001  Simon Wistow <simon@twoshortplanks.com>
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *
 * $Log: swf_tags.c,v $
 * Revision 1.1  2001/07/12 11:28:14  muttley
 * Initial revision
 * Removed all the tags from swf_types.h and put them in here
 * Wrote function to convert tag code to string
 *
 */

#include "swf_tags.h"

const char * swf_tags [] = {
        "End",
        "ShowFrame",
        "DefineShape",
        "FreeCharacter",
        "PlaceObject",
        "RemoveObject",
        "DefineBits",
        "DefineButton",
        "JPEGTables",
        "SetBackgroundColor",
        "DefineFont",
        "DefineText",
        "DoAction",
        "DefineFontInfo",
        "DefineSound",         /* Event sound tags. */
        "StartSound",
        "",
        "DefineButtonSound",
        "SoundStreamHead",
        "SoundStreamBlock",
        "DefineBitsLossless",  /* A bitmap using lossless zlib compression. */
        "DefineBitsJPEG2",     /* A bitmap using an internal JPEG compression table. */
        "DefineShape2",
        "DefineButtonCxform",
        "Protect",             /* This file should not be importable for editing. */
        "",

        /*  These are the new tags for Flash 3. */
        "PlaceObject2",        /* The new style place w/ alpha color transform and name. */
        "",
        "RemoveObject2",       /* A more compact remove object that omits the character tag (just depth). */
        "",
        "",
        "",
        "DefineShape3",        /* A shape V3 includes alpha values. */
        "DefineText2",         /* A text V2 includes alpha values. */
        "DefineButton2",       /* A button V2 includes color transform, alpha and multiple actions */
        "DefineBitsJPEG3",     /* A JPEG bitmap with alpha info. */
        "DefineBitsLossless2", /* A lossless bitmap with alpha info. */
        "DefineEditText",      /* An editable Text Field */
        "",
        "DefineSprite",        /* Define a sequence of tags that describe the behavior of a sprite. */
        "NameCharacter",       /* Name a character definition, character id and a string, (used for buttons, bitmaps, sprites and sounds). */
        "",
        "",
        "FrameLabel",          /* A string label for the current frame. */
        "",
        "SoundStreamHead2",    /* For lossless streaming sound, should not have needed this... */
        "DefineMorphShape",    /* A morph shape definition */
        "",
        "DefineFont2"
};


const char *
swf_tag_to_string (int id)
{
    return swf_tags[id];

}

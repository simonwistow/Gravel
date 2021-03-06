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
 * $Log: print_utils.c,v $
 * Revision 1.21  2002/07/03 08:05:20  kitty_goth
 * Add support for gradient fills in generation. These need to be tested along
 * with curves. New test script. Fix some segfaults (still weirdness in
 * lossless, though). Remove U32 instead of proper colour nonsense.
 *
 * Revision 1.20  2002/06/28 17:52:42  muttley
 * Trying to get write_through to work
 *
 * Revision 1.19  2002/06/20 17:02:15  kitty_goth
 * Coloured fills work properly. Got rid of the SWF_U32 nasty representation
 * of colours.
 *
 * Revision 1.18  2002/06/08 13:47:30  kitty_goth
 * This is still a bit broken but defineshape generation is actually starting to
 * produce stuff
 *
 * Revision 1.17  2002/06/08 12:25:42  kitty_goth
 * Modify swf_parse to pick out the define shapes only. Have a read of these
 * changes when you get a minute, Simon.
 *
 * Revision 1.16  2002/06/07 17:18:00  kitty_goth
 * Well, it is *producing* a defineShape - it's just a bit mangled.
 * We need to move away from icky SWF_U32 pretending to be colours
 *
 * Revision 1.15  2002/05/09 15:26:55  clampr
 * More stealing from perl, now we have pSWF_* macros which are the printf
 * formats for out internal data types.
 *
 * Plus a few -Wall cleanups
 *
 * Revision 1.14  2001/07/16 14:14:40  clampr
 * free of null
 *
 * Revision 1.13  2001/07/15 15:50:37  clampr
 * line deltas a gogo
 *
 * Revision 1.12  2001/07/13 13:26:55  muttley
 * Added handling for button2actions.
 * We should be able to parse all URLs now
 *
 * Revision 1.11  2001/07/09 15:48:54  acme
 * Renamed U32 to SWF_U32 and so on
 *
 * Revision 1.10  2001/07/09 12:47:59  muttley
 * Changes for lib_swfextract and text_extract
 *
 * Revision 1.8  2001/06/29 15:10:11  muttley
 * The printing of the actual text of a DefineText (and DefineText2 now)
 * is no longer such a big hack. Font information is kept in the swf_parser
 * context and the function that will take a text_record_list and print out
 * the text (textrecord_list_to_text) has been moved to swf_parse.c ...
 *
 * A couple of potential bugs have also been fixed and some more 'todo's added
 *
 * Revision 1.7  2001/06/26 13:45:03  muttley
 * Add facilities for converting DefineFontInfo, DefineFont and DefineText info into 'real' text
 *
 */

#include "print_utils.h"

void
print_soundstreamhead (swf_soundstreamhead * head, const char * str, int is_head2)
{

    const char* compression[3] = {"uncompressed", "ADPCM", "MP3"};
    const char* sample_rate[4] = {"5.5", "11", "22", "44"};
    const char* sample_size    = (head->stream_sample_size == 0 ? "8" : "16");
    const char* stereo_mono    = (head->stream_sample_stereo_mono == 0 ? "mono" : "stereo");

    /* todo simon : change sample_stereo_mono */

    if (head==NULL) {
        return;
    }

    printf("%stagSoundStreamHead%s: %s %skHz %s-bit %s AverageSamplesPerFrame:%d\n",
		   str, is_head2 ? "2" : "", compression[head->stream_compression], sample_rate[head->stream_sample_rate],
		   sample_size, stereo_mono, head->n_stream_samples);
}

void
print_mp3header_list (swf_mp3header_list * header_list,const char * str) {
    int i;

    if (header_list==NULL) {
        return;
    }

    for (i=0; i<header_list->header_count; i++)
    {
		printf("%s  Frame%d: MPEG%s Layer%d %dHz %s %dbps size:Encoded:%d Decoded:%d\n",
			   str, i+1, swf_MPEG_Ver[header_list->headers[i]->ver], header_list->headers[i]->layer+1, header_list->headers[i]->freq,
			   (header_list->headers[i]->stereo) ? "stereo" : "mono", header_list->headers[i]->rate,
			   header_list->headers[i]->encoded_frame_size, header_list->headers[i]->decoded_frame_size);
    }
}

void
print_adpcm (swf_adpcm * adpcm,const char * str) {
    /* todo simon */
}

void
print_buttonrecords (swf_buttonrecord_list * list,const char * str)
{
    swf_buttonrecord * node;
    int j;

    if (list==NULL){
        return;
    }

    node = list->first;

    while (node != NULL)
    {
		printf("%s\tParseButtonRecord: char:%"pSWF_U32" layer:%"pSWF_U32" ", str, node->character, node->layer);

		if (node->state_hit_test != 0)  printf("HIT ");
		if (node->state_down != 0)      printf("DOWN ");
		if (node->state_over != 0)      printf("OVER ");
		if (node->state_up != 0)        printf("UP ");

		printf("\n");


		print_matrix (node->matrix, str);
		
		for (j=0; j<node->ncharacters; j++)
		{
			print_cxform(node->characters[j], str);
		}

		node = node->next;
    }
}

void
print_doactions (swf_doaction_list * list, const char * str)
{
    swf_doaction * node;

    if (list==NULL)
    {
        return;
    }

    node = list->first;

    while (node != NULL)
    {
        print_doaction (node, str);
        node = node->next;
    }
}

void
print_doaction (swf_doaction * action, const char * str)
{
	INDENT;
	printf("%saction code 0x%02x ", str, action->code);
	if (action->code == 0)
	{
		/* Action code of zero indicates end of actions */
		printf("\n");
		return;
	}

	switch ( action->code )
	{
	case sactionNextFrame:
		printf( "gotoNextFrame\n" );
		break;

	case sactionPrevFrame:
		printf( "gotoPrevFrame\n" );
		break;

	case sactionPlay:
		printf( "play\n" );
		break;

	case sactionStop:
		printf( "stop\n" );
		break;

	case sactionToggleQuality:
		printf( "toggleQuality\n" );
		break;

	case sactionStopSounds:
		printf( "stopSounds\n" );
		break;

	case sactionAdd:
		printf( "add\n" );
		break;

	case sactionSubtract:
		printf( "subtract\n" );
		break;

	case sactionMultiply:
		printf( "multiply\n" );
		break;
		
	case sactionDivide:
		printf( "divide\n" );
		break;

	case sactionEqual:
		printf( "equal\n" );
		break;

	case sactionLessThan:
		printf( "lessThan\n" );
		break;

	case sactionLogicalAnd:
		printf( "logicalAnd\n" );
		break;

	case sactionLogicalOr:
		printf( "logicalOr\n" );
		break;

	case sactionLogicalNot:
		printf( "logicalNot\n" );
		break;

	case sactionStringEqual:
		printf( "stringEqual\n" );
		break;

	case sactionStringLength:
		printf( "stringLength\n" );
		break;

	case sactionSubString:
		printf( "subString\n" );
		break;

	case sactionInt:
		printf( "int\n" );
		break;

	case sactionEval:
		printf( "eval\n" );
		break;

	case sactionSetVariable:
		printf( "setVariable\n" );
		break;

	case sactionSetTargetExpression:
		printf( "setTargetExpression\n" );
		break;

	case sactionStringConcat:
		printf( "stringConcat\n" );
		break;

	case sactionGetProperty:
		printf( "getProperty\n" );
		break;
        
	case sactionSetProperty:
		printf( "setProperty\n" );
		break;
	
	case sactionDuplicateClip:
		printf( "duplicateClip\n" );
		break;

	case sactionRemoveClip:
		printf( "removeClip\n" );
		break;
	
	case sactionTrace:
		printf( "trace\n" );
		break;
	
	case sactionStartDragMovie:
		printf( "startDragMovie\n" );
		break;

	case sactionStopDragMovie:
		printf( "stopDragMovie\n" );
		break;

	case sactionStringLessThan:
		printf( "stringLessThan\n" );
		break;
	
	case sactionRandom:
		printf( "random\n" );
		break;
	
	case sactionMBLength:
		printf( "mbLength\n" );
		break;
	
	case sactionOrd:
		printf( "ord\n" );
		break;

	case sactionChr:
		printf( "chr\n" );
		break;

	case sactionGetTimer:
		printf( "getTimer\n" );
		break;

	case sactionMBSubString:
		printf( "mbSubString\n" );
		break;

	case sactionMBOrd:
		printf( "mbOrd\n" );
		break;

	case sactionMBChr:
		printf( "mbChr\n" );
		break;

	case sactionGotoFrame:
		printf("gotoFrame %5u\n",action->frame);
		break;

	case sactionGetURL:
		printf("getUrl %s target %s\n", action->url, action->target);
		break;

	case sactionWaitForFrame:
		printf("waitForFrame %-5u skipCount %-5u\n", action->frame, action->skip_count);
		break;

	case sactionSetTarget:
		printf("setTarget %s\n", action->target);
		break;

	case sactionGotoLabel:
		// swfparse used to crash here!
		printf("gotoLabel %s\n", action->goto_label);
		break;

	case sactionWaitForFrameExpression:
		printf( "waitForFrameExpression skipCount %-5u\n", action->skip_count );
		break;

	case sactionPushData:
		/* property ids are pushed as floats for some reason */
		if ( action->push_data_type == 1 )
		{
			printf("pushData (float): %08"pSWF_U32"x %.1f\n", action->push_data_float.dw, action->push_data_float.f);
		}
		else
			if ( action->push_data_type == 0 )
			{
				printf("pushData (string): %s\n", action->push_data_string);
			}
			else
			{
				printf( "pushData invalid dataType: %02"pSWF_U32"x\n", action->push_data_type);
			}
		break;

	case sactionBranchAlways:
		printf("branchAlways offset: %-5u\n",action->branch_offset);
		break;

	case sactionGetURL2:
		if ( action->url2_flag == 1 )
		{
			printf( "getUrl2 sendvars=GET\n" );
		}
		else
			if ( action->url2_flag == 2 )
			{
				printf( "getUrl2 sendvars=POST\n" );
			}
			else
			{
				printf( "getUrl2 sendvars=Don't send\n" );
			}
		break;

	case sactionBranchIfTrue:
		printf("branchIfTrue offset: %-5u\n", action->branch_offset);
		break;

	case sactionCallFrame:
		printf( "callFrame\n" );
		break;

	case sactionGotoExpression:
		if ( action->stop_flag == 0 )
		{
			printf("gotoExpression and Stop\n" );
		}
		else
			if ( action->stop_flag == 1 )
			{
				printf("gotoExpression and Play\n" );
			}
			else
			{
				printf("gotoExpression invalid stopFlag: %d\n", action->stop_flag );
			}
		break;


	default:
		printf("UNKNOWN?\n");
		break;
	}

}

void
print_button2actions (swf_button2action_list * list,const char * str)
{
    swf_button2action * node;

    if (list==NULL)
    {
        return;
    }

    node = list->first;

    while (node != NULL)
    {
        print_button2action (node, str);
        node = node->next;
    }
}

void
print_button2action (swf_button2action * action, const char * str)
{
     printf("%sCondition: %04"pSWF_U32"x\n", str, action->condition);
     print_doactions (action->doactions, str);
}



void
print_textrecord (swf_textrecord * node, const char * str)
{
    int g;

    printf("\n%s\tflags: 0x%02x\n", str, node->flags);

    if (node->flags & isTextControl)
    {
        if (node->flags & textHasFont)
        {
            printf("%s\tfontId: %ld\n", str, node->font_id);

        }

        if (node->flags & textHasColour)
        {
            printf("%s\tfontColour: %06"pSWF_U32"x\n", str, node->colour);
        }

        if (node->flags & textHasXOffset)
        {

            printf("%s\tX-offset: %d\n", str, node->xoffset);
        }
        if (node->flags & textHasYOffset)
        {
            printf("%s\tY-offset: %d\n", str, node->yoffset);
        }
        if (node->flags & textHasFont)
        {

            printf("%s\tFont Height: %d\n", str, node->font_height);
        }
    }
    else
    {
        printf("%s\tnumber of glyphs: %d\n", str, node->glyph_count);

        printf("%s\t", str);

        for (g = 0; g < node->glyph_count; g++)
        {
            printf("[%d,%d] ", node->glyphs[g][0], node->glyphs[g][1]);
        }
    }
}




void
print_textrecords (swf_textrecord_list * list, const char * str, swf_parser * context)
{
    swf_textrecord * tmp;
    swf_textrecord * node;
    int error = SWF_ENoError;
    char * text = NULL;

    text = swf_parse_textrecords_to_text(context, &error, list);

    node = list->first;

    while (node != NULL)
    {
        tmp = node;
        node = node->next;
        print_textrecord (tmp, str);
    }

    if (text!=NULL) {
        printf ("%s\ttext representation : %s\n", str, text);
    }
    swf_free (text);
}

void
print_rect (swf_rect * rect,const char * str)
{
    if (rect==NULL) {
        return;
    }

    printf("\t%s(%g, %g)[%g x %g]\n", str,
          (double) rect->xmin / 20.0, (double)rect->ymin / 20.0,
          (double)(rect->xmax - rect->xmin) / 20.0,
          (double)(rect->ymax - rect->ymin) / 20.0);
}


void
print_imageguts (swf_imageguts * guts,const char * str)
{
    SWF_U32 nbytes   = 0;

    INDENT;
    printf("%s----- dumping image details -----", str);

    if (guts==NULL) {
        printf("\n");
        return;
    }

    while (nbytes<guts->nbytes)
    {
        //if (guts->data[nbytes] == NULL) {
        //    break;
        //}

        if ((nbytes % 16) == 0)
        {
            printf("\n");
            INDENT;
            printf("%s", str);
        }
        printf("%02x ", guts->data[nbytes]);
        nbytes ++;

    }
    printf("\n");

    return;
}


void
print_startsound (swf_startsound * sound,const char * str)
{
    int i = 0;

    if (sound == NULL) {
        fprintf (stderr, "ERROR : couldn't parse start sound\n");
        exit (1);
    }


    INDENT;
    printf("%scode %-3"pSWF_U32, str, sound->code);

    if ( sound->code & soundHasInPoint ) {
        printf(" inpoint %"pSWF_U32" ", sound->inpoint);
    } if ( sound->code & soundHasOutPoint ) {
        printf(" outpoint %"pSWF_U32, sound->outpoint);
    } if ( sound->code & soundHasLoops ) {
        printf(" loops %"pSWF_U32, sound->loops);
    }

    printf("\n");
    if ( sound->code & soundHasEnvelope )
    {
        for (i = 0; i < sound->npoints && sound->points != NULL; i++ )
        {
            printf("\n");
            INDENT;
            printf("%smark44 %"pSWF_U32, str,  sound->points[i]->mark);
            printf(" left chanel %"pSWF_U32, sound->points[i]->lc);
            printf(" right chanel %"pSWF_U32, sound->points[i]->rc);
            printf("\n");
        }
    }
}

void
print_shapestyle (swf_shapestyle * style,const char * str)
{
    SWF_U16 i = 0, j = 0;

    if (style==NULL) {
		printf("Bad style, man\n");
        return;
    }

    printf("%s\tNumber of fill styles \t%u\n", str, style->nfills);

    /* Get each of the fill style. */
    for (i = 0; i < style->nfills; i++) {
        if (style->fills[i]->fill_style & fillGradient) {

            /* Get the number of colors. */
            printf("%s\tGradient Fill with %u colors\n", str, style->fills[i]->ncolours);

            /* Get each of the colors. */
            for (j = 0; j < style->fills[i]->ncolours; j++) {
                printf("%s\tcolor:%d: at:%d  RGBA:%02"pSWF_U8":%02"pSWF_U8":%02"pSWF_U8":x\n", str, j, style->fills[i]->colours[j]->pos, style->fills[i]->colours[j]->col->r, style->fills[i]->colours[j]->col->g, style->fills[i]->colours[j]->col->b);
            }
            printf("%s\tGradient Matrix:\n", str);
            print_matrix(style->fills[i]->matrix, str);
        }
        else if (style->fills[i]->fill_style & fillBits)
        {
            /* Get the bitmap matrix. */
            printf("%s\tBitmap Fill: %04x\n", str, style->fills[i]->bitmap_id);
            print_matrix(style->fills[i]->matrix, str);
        } else {
            /* A solid color */
            printf("%s\tSolid Color Fill in R:G:B HEX: %02"pSWF_U8":%02"pSWF_U8":%02"pSWF_U8":x\n", str, style->fills[i]->col->r, style->fills[i]->col->g, style->fills[i]->col->b);
        }
    }

    printf("%s\tNumber of line styles \t%u\n", str, style->nlines);

    /* Get each of the line styles. */
    for (i = 0; i < style->nlines && style->lines[i] != NULL; i++) {
        printf("%s\tLine style %-5u width %g color RGB_HEX %02"pSWF_U8":%02"pSWF_U8":%02"pSWF_U8":x\n", str, i+1, (double)style->lines[i]->width/20.0, style->lines[i]->col->r, style->lines[i]->col->g, style->lines[i]->col->b);
    }
}

void
print_shaperecords (swf_shaperecord_list * record,const char * str)
{
	swf_shaperecord *node, *temp;

	node = record->first;
	
	while (node != NULL) {
		temp = node;
		node = node->next;
    
		printf("%s\tShape isEdge: %i ; Flags : %u\n", str, temp->is_edge, temp->flags);
		if (temp->flags & eflagsMoveTo) {
			printf("%s\t\tMove To\n", str);
		}
		if (temp->flags & eflagsFill0) {
			printf("%s\t\tFill 0 Changes to %i\n", str, temp->fillstyle0);
		}
		if (temp->flags & eflagsFill1) {
			printf("%s\t\tFill 1 Changes to %i\n", str, temp->fillstyle1);
		}
		if (temp->flags & eflagsLine) {
			printf("%s\t\tLine Changes\n", str);
		}
		if (temp->flags & eflagsNewStyles) {
			printf("%s\t\tStyles Change\n", str);
		}

		if (temp->x | temp->y) {
			//printf("%s\tLine Found : x : %li ; y : %li\n", str, temp->x, temp->y);
			printf("%s\tLine Found : x : %"pSWF_S32" ; y : %"pSWF_S32"\n", str, temp->x, temp->y);
	                
		}
		if (temp->ax | temp->ay | temp->cx | temp->cy) {
			printf("%s\tCurve Found : ax : %"pSWF_S32" ; ay : %"pSWF_S32"; cx : %"pSWF_S32" ; cy : %"pSWF_S32"\n", str, temp->ax, temp->ay, temp->cx, temp->cy);
		}
	}
}


void
print_cxform (swf_cxform * cxform,const char * str)
{
    if (cxform==NULL) {
        return;
    }

    printf("%sCXFORM:\n", str);
    printf("%sAlpha:  mul:%04u  add:%04u\n", str, cxform->aa, cxform->ab);
    printf("%sRed:    mul:%04u  add:%04u\n", str, cxform->ra, cxform->rb);
    printf("%sGreen:  mul:%04u  add:%04u\n", str, cxform->ga, cxform->gb);
    printf("%sBlue:   mul:%04u  add:%04u\n", str, cxform->ba, cxform->bb);
}


void
print_matrix (swf_matrix * matrix,const char * str)
{
    if (matrix==NULL) {
        return;
    }
    printf("%s\t[%5.3f   %5.3f]\n", str,  (double)matrix->a/65536.0, (double)matrix->b/65536.0);
    printf("%s\t[%5.3f   %5.3f]\n", str,  (double)matrix->c/65536.0, (double)matrix->d/65536.0);
    printf("%s\t[%5.3f   %5.3f]\n", str,  (double)matrix->tx/20.0,   (double)matrix->ty/20.0);
}



/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/




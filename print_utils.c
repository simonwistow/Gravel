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

    return;

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
    return;

}

void
print_adpcm (swf_adpcm * adpcm,const char * str) {
    /* todo simon */
    return;

}

void
print_buttonrecords (swf_buttonrecord_list * list,const char * str)
{
    int i, j;

    if (list==NULL){
        return;
    }

    for (i=0; i<list->record_count; i++) {

            printf("%s\tParseButtonRecord: char:%ld layer:%ld ", str, list->records[i]->character, list->records[i]->layer);


            if (list->records[i]->state_hit_test != 0)  printf("HIT ");
            if (list->records[i]->state_down != 0)      printf("DOWN ");
            if (list->records[i]->state_over != 0)      printf("OVER ");
            if (list->records[i]->state_up != 0)        printf("UP ");


            printf("\n");


            print_matrix (list->records[i]->matrix, str);

            for (j=0; j<list->records[i]->ncharacters; j++)
            {

                    //print_cxform(list->records[i]->characters[j], str);

            }



    }

    return;
}

void
print_doactions (swf_doaction_list * actions,const char * str)
{
    //todo simon
    return;

}

void
print_button2actions (swf_button2action_list * actions,const char * str)
{
	//todo simon
	return;
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
            printf("%s\tfontColour: %06lx\n", str, node->colour);
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
    free (text);
    return;
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


    return;

}


void
print_imageguts (swf_imageguts * guts,const char * str)
{

    U32 nbytes   = 0;



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
    printf("%scode %-3lu", str, sound->code);

    if ( sound->code & soundHasInPoint ) {
        printf(" inpoint %lu ", sound->inpoint);
    } if ( sound->code & soundHasOutPoint ) {
        printf(" outpoint %lu", sound->outpoint);
    } if ( sound->code & soundHasLoops ) {
        printf(" loops %lu", sound->loops);
    }

    printf("\n");
    if ( sound->code & soundHasEnvelope )
    {
        for (i = 0; i < sound->npoints && sound->points != NULL; i++ )
        {
            printf("\n");
            INDENT;
            printf("%smark44 %lu", str,  sound->points[i]->mark);
            printf(" left chanel %lu", sound->points[i]->lc);
            printf(" right chanel %lu", sound->points[i]->rc);
            printf("\n");
        }
    }


    return;

}

void
print_shapestyle (swf_shapestyle * style,const char * str)
{
    U16 i = 0, j = 0;

    if (style==NULL) {
        return;
    }

    printf("%s\tNumber of fill styles \t%u\n", str, style->nfills);

    /* Get each of the fill style. */
    for (i = 0; i < style->nfills; i++)
    {


        if (style->fills[i]->fill_style & fillGradient)
        {

            /* Get the number of colors. */
            printf("%s\tGradient Fill with %u colors\n", str, style->fills[i]->ncolours);

            /* Get each of the colors. */
            for (j = 0; j < style->fills[i]->ncolours; j++)
            {
                printf("%s\tcolor:%d: at:%d  RGBA:%08lx\n", str, j, style->fills[i]->colours[j]->pos, style->fills[i]->colours[j]->rgba);
            }
            printf("%s\tGradient Matrix:\n", str);
            print_matrix(style->fills[i]->matrix, str);
        }
        else if (style->fills[i]->fill_style & fillBits)
        {
            /* Get the bitmap matrix. */
            printf("%s\tBitmap Fill: %04x\n", str, style->fills[i]->bitmap_id);
            print_matrix(style->fills[i]->matrix, str);
        }
        else
        {
            /* A solid color */
            printf("%s\tSolid Color Fill RGB_HEX %06lx\n", str, style->fills[i]->colour);
        }
    }

    printf("%s\tNumber of line styles \t%u\n", str, style->nlines);

    /* Get each of the line styles. */
    for (i = 0; i < style->nlines; i++)
    {
        printf("%s\tLine style %-5u width %g color RGB_HEX %06lx\n", str, i+1, (double)style->lines[i]->width/20.0, style->lines[i]->colour);
    }


    return;
}

void
print_shaperecords (swf_shaperecord_list * record,const char * str)
{
    //todo
    return;
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

    return;
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

    return;

}





/*
   $Id: png_wrapper.cc,v 1.1 2006/04/07 16:12:59 Mithander Exp $

   Copyright (C) 2006   Tyler Nielsen <tyler.nielsen@gmail.com>
   Part of the Adonthell Project http://adonthell.linuxgames.com

   Adonthell is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Adonthell is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Adonthell; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


/**
 * @file   png.cc
 * @author Tyler Nielsen <tyler.nielsen@gmail.com>
 *
 * @brief  Defines the png static class.
 *
 *
 */

#include "png_wrapper.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <png.h>

using namespace std;

namespace gfx
{
    // Functions for overriding reading/writing png using iostreams
    void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
    {
        ifstream &file = *(ifstream *)(png_get_io_ptr(png_ptr));
        file.read((char *)data,length);
    }

    void user_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
    {
        ofstream &file = *(ofstream*)(png_get_io_ptr(png_ptr));
        file.write((char *)data, length);
    }

    void user_flush_data(png_structp png_ptr)
    {
        ofstream &file = *(ofstream*)(png_get_io_ptr(png_ptr));
        file.flush();
    }

    void * png::get (ifstream & file, u_int16 & length, u_int16 & height)
    {
        const int headerbytes = 8;  //This is used to read the file and make sure its a png... can be 1-8
        png_byte header[headerbytes];
        png_structp png_ptr;
        png_infop info_ptr;

        int number_of_passes;
        png_bytep * row_pointers;

        /* open file and test for it being a png */
        file.read((char *)header, headerbytes);
        if (png_sig_cmp(header, 0, headerbytes))
        {
            cout << "[read_png_file] File %s is not recognized as a PNG file" << endl;
            return NULL;
        }

        /* initialize stuff */
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_ptr)
        {
            cout << "[read_png_file] png_create_read_struct failed" << endl;
            return NULL;
        }

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
        {
            cout << "[read_png_file] png_create_info_struct failed" << endl;
            png_destroy_read_struct(&png_ptr, NULL, NULL);
            return NULL;
        }

        if (setjmp(png_jmpbuf(png_ptr)))
        {
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            cout << "[read_png_file] Error during init_io" << endl;
            return NULL;
        }

        png_set_read_fn(png_ptr, &file, user_read_data);
        png_set_sig_bytes(png_ptr, headerbytes);

        png_read_info(png_ptr, info_ptr);

        length = info_ptr->width;
        height = info_ptr->height;

        number_of_passes = png_set_interlace_handling(png_ptr);
        png_read_update_info(png_ptr, info_ptr);

        /* read file */
        if (setjmp(png_jmpbuf(png_ptr)))
        {
            cout << "[read_png_file] Error during read_image" << endl;
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            return NULL;
        }

        row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
        for (int y=0; y<height; y++)
            row_pointers[y] = (png_byte*) malloc(info_ptr->rowbytes);

        png_read_image(png_ptr, row_pointers);

        char *image = image = (char *)calloc (length * height, 3);
        int idx = 0;

        switch (info_ptr->color_type)
        {
        case PNG_COLOR_TYPE_RGBA:
        case PNG_COLOR_TYPE_RGB:
            for (int y=0; y<height; y++)
            {
                png_byte* row = row_pointers[y];
                for (int x=0; x<length; x++)
                {
                    image[idx++] = *row++;
                    image[idx++] = *row++;
                    image[idx++] = *row++;
                    if (info_ptr->color_type == PNG_COLOR_TYPE_RGBA)
                    {
                        //TODO Use alpha channel also
                        row++;
                    }
                }
                free(row_pointers[y]);
            }
            free(row_pointers);
            break;
        default:
            cout << "[read_png_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (is " << info_ptr->color_type << ")" << endl;
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            return NULL;
        }

        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return image;
    }

    void png::put (ofstream & file, const char *image, u_int16 length, u_int16 height)
    {
        png_structp png_ptr;
        png_infop info_ptr;
        png_bytep * row_pointers;

        /* initialize stuff */
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_ptr)
        {
            cout << "[write_png_file] png_create_write_struct failed" << endl;
            return;
        }

    	info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
        {
            cout << "[write_png_file] png_create_info_struct failed" << endl;
            png_destroy_write_struct(&png_ptr, NULL);
            return;
        }

        if (setjmp(png_jmpbuf(png_ptr)))
        {
            cout << "[write_png_file] Error during init_io" << endl;
            png_destroy_write_struct(&png_ptr, &info_ptr);
            return;
        }

        png_set_write_fn(png_ptr, &file, user_write_data, user_flush_data);

    	/* write header */
        if (setjmp(png_jmpbuf(png_ptr)))
        {
            cout << "[write_png_file] Error during writing header" << endl;
            png_destroy_write_struct(&png_ptr, &info_ptr);
            return;
        }

        png_set_IHDR(png_ptr, info_ptr, length, height,
            8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        png_write_info(png_ptr, info_ptr);

        /* write bytes */
        if (setjmp(png_jmpbuf(png_ptr)))
        {
            cout << "[write_png_file] Error during writing bytes" << endl;
            png_destroy_write_struct(&png_ptr, &info_ptr);
            return;
        }

        int idx = 0;
        row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
        for (int y=0; y<height; y++)
        {
            row_pointers[y] = (png_byte*) malloc(info_ptr->rowbytes);
            png_byte* row = row_pointers[y];
            for (int x=0; x<length; x++)
            {
                *row++ = image[idx++];
                *row++ = image[idx++];
                *row++ = image[idx++];
            }
        }

        png_write_image(png_ptr, row_pointers);
        png_write_end(png_ptr, NULL);

        for (int y=0; y<height; y++)
            free(row_pointers[y]);
        free(row_pointers);

        png_destroy_write_struct(&png_ptr, &info_ptr);
    }
}
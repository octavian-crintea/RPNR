//
//  IO.cpp
//  rpnr
//
//  Created by Octavian Crintea on 11/15/14.
//  Copyright (c) 2014 Octavian Crintea. All rights reserved.
//

#include "IO.h"
#include "LMImageRepresentation.h"

#include <string>
#include <png.h>

#define PNG_HEADER_LENGTH 8

static std::string buildPath(const char* workingPath, const char* fileName)
{
    std::string filePath = workingPath;
    if (filePath != "" && filePath.at(filePath.size() - 1) != '/') {
        filePath += '/';
    }
    filePath += fileName;

    return filePath;
}

LMImageRepresentation* loadImageData(const char* workingPath, const char* imageName)
{
    FILE* imageFile = fopen(buildPath(workingPath, imageName).c_str(), "rb");
    if (imageFile == NULL) {
        return NULL;
    }

    uint8_t header[PNG_HEADER_LENGTH];
    fread(header, 1, PNG_HEADER_LENGTH, imageFile);

    if (ferror(imageFile)) {
        return NULL;
    }

    if (png_sig_cmp(header, 0, PNG_HEADER_LENGTH) != 0) {
        return NULL;
    }

    png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (pngPtr == NULL) {
        return NULL;
    }

    png_infop infoPtr = png_create_info_struct(pngPtr);
    if (infoPtr == NULL) {
        return NULL;
    }

    if (setjmp(png_jmpbuf(pngPtr)) != 0) {
        return NULL;
    }
    png_init_io(pngPtr, imageFile);

    png_set_sig_bytes(pngPtr, PNG_HEADER_LENGTH);

    if (setjmp(png_jmpbuf(pngPtr))) {
        return NULL;
    }
    png_read_info(pngPtr, infoPtr);

    uint32_t height = png_get_image_height(pngPtr, infoPtr);
    uint32_t widthBytes = png_get_rowbytes(pngPtr, infoPtr);
    uint8_t channels = png_get_channels(pngPtr, infoPtr);
    uint8_t bitDepth = png_get_bit_depth(pngPtr, infoPtr);

    uint8_t** rowPtrs = (uint8_t**) malloc(height * sizeof(uint8_t*));
    if (rowPtrs == NULL) {
        return NULL;
    }

    uint8_t* matrix = (uint8_t*) malloc(widthBytes * height);
    if (matrix == NULL) {
        return NULL;
    }

    for (uint32_t i = 0; i < height; i++) {
        rowPtrs[i] = matrix + i * widthBytes;
    }

    if (setjmp(png_jmpbuf(pngPtr))) {
        return NULL;
    }
    png_read_image(pngPtr, rowPtrs);

    if (setjmp(png_jmpbuf(pngPtr))) {
        return NULL;
    }
    png_read_end(pngPtr, infoPtr);

    LMImageRepresentation* image =
        new LMImageRepresentation(matrix,
                                  bitDepth * channels,
                                  widthBytes,
                                  widthBytes * height,
                                  channels);

    free(matrix);
    free(rowPtrs);
    png_destroy_info_struct(pngPtr, &infoPtr);
    png_destroy_read_struct(&pngPtr, NULL, NULL);
    fclose(imageFile);

    return image;
}

bool saveImageData(LMImageRepresentation* rep, const char* workingPath, const char* imageName)
{
    FILE* imageFile = fopen(buildPath(workingPath, imageName).c_str(), "wb");
    if (imageFile == NULL) {
        return false;
    }

    png_structp pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (pngPtr == NULL) {
        return false;
    }

    png_infop infoPtr = png_create_info_struct(pngPtr);
    if (infoPtr == NULL) {
        return false;
    }

    if (setjmp(png_jmpbuf(pngPtr)) != 0) {
        return false;
    }
    png_init_io(pngPtr, imageFile);

    png_set_IHDR(pngPtr,
                 infoPtr,
                 rep->_width,
                 rep->_height,
                 rep->_bitsPerPixel / rep->_samplesPerPixel,
                 rep->_samplesPerPixel - 1, // not quite sure, please verify the
                                            // relation between color_type and
                                            // channels (samplesPerPixel)
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);

    if (setjmp(png_jmpbuf(pngPtr))) {
        return false;
    }
    png_write_info(pngPtr, infoPtr);

    uint8_t** rowPtrs = (uint8_t**) malloc(rep->_height * sizeof(uint8_t*));
    if (rowPtrs == NULL) {
        return false;
    }

    for (uint32_t i = 0; i < rep->_height; i++) {
        rowPtrs[i] = rep->_bytes + i * rep->_width * rep->_samplesPerPixel;
    }

    if (setjmp(png_jmpbuf(pngPtr))) {
        return false;
    }
    png_write_image(pngPtr, rowPtrs);

    if (setjmp(png_jmpbuf(pngPtr))) {
        return false;
    }
    png_write_end(pngPtr, infoPtr);

    free(rowPtrs);
    png_destroy_info_struct(pngPtr, &infoPtr);
    png_destroy_write_struct(&pngPtr, NULL);
    fclose(imageFile);

    return true;
}

bool testIO(const char* workingPath, const char* inFile, const char *outFile)
{
    LMImageRepresentation* rep = loadImageData(workingPath, inFile);
    if (rep == NULL) {
        return false;
    }

    bool rs = saveImageData(rep, workingPath, outFile);
    delete rep;

    return rs;
}

/********************************************************************************
  $Header$
*/
/*
  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    23/Jan/2002 - Dirk Farin - Complete reimplementation based on old Bitmap type.
    02/Jun/1999 - Dirk Farin - First implementation based on DVDview code.
 ********************************************************************************
    LibVideoGfx - video processing library
    Copyright (C) 2002  Dirk Farin

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_BASIC_BITMAP_HH
#define LIBVIDEOGFX_GRAPHICS_BASIC_BITMAP_HH

#include <libvideogfx/types.hh>
#include <libvideogfx/utility/math.hh>
#include <string.h>
#include <algorithm>
#include <assert.h>

namespace videogfx {

  template <class Pel> class BitmapProvider;

  /** Bitmap data-type definition.
      A Bitmap can be used for any two-dimensional image data using
      arbitrary types for the pixels. In that sense, a Bitmap is nothing
      but a two-dimensional array. What makes the Bitmap special is
      <ul>
      <li>automatic reference-counter based memory handling, making
          copying efficient,
      <li>transparent alignment of bitmap sizes to integer multiples
          of some constants (e.g., 16 pixels to simplify MPEG coders),
      <li>transparent border around the bitmap to allow algorithms,
          that consider a context around a current pixel, to ignore
          boundary conditions,
      <li>multiple views into a single bitmap, to use a small part
          of a bitmap like a bitmap of its own, or to have acces to
          interlaced fields just like full-resolution frames (which
          for example simplifies MPEG field-MC),
      <li>ability to wrap fixed memory ranges (like your gfx-card
          memory) into a Bitmap (not written yet).
      </ul>

      Internally, Bitmap is only the \em envelope class to the \em letter
      class BitmapProvider, which contains the actual data. When a bitmap
      is copied, the data is not copied! Instead, copying will result in
      a second Bitmap-envelope object using the same bitmap data. Changes
      made to one bitmap will hence also influence the other. If you want
      to make an independent copy of your bitmap, use Clone() instead.

      Usually, a Bitmap is created in main memory by specifying its size
      and optionally alignment and a surrounding border.
      For special purpose applications, BitmapProviders can be defined which
      wrap the Bitmap-API around existing memory blocks.

      When creating bitmaps in main memory, optionally to the bitmap size, a
      border and horizontal, vertical alignments can be specified. The border
      is a safety memory around the actual bitmap data. I.e., you may access
      bitmap content if indexes in the range \f$[-border; width+border[\f$.
      An alignment rounds the bitmap up to a multiple of the alignment factor
      (but not necessarily the next multiple). Be warned that you may get
      differing values for border and alignment back from the bitmap class.
      However, these will never be smaller than the values you specified.
      Horizontal border sizes are added before doing the alignment. Hence,
      the total width and stride (number of data elements, usually bytes,
      between two lines) will be a multiple of the horizontal alignment.
      \image latex bitmap.eps "bitmap border, alignments and corresponding sizes", width=8cm
      \image html bitmap.png  "bitmap border, alignments and corresponding sizes"
      The alignment layout and the involved widths (height are named similarly) are
      depicted in the above figure. The border size is aligned to the horizontal
      alignment factor only. Vertical border size is not increased.
      Note that even though the border is here shown
      as beginning behind the alignment at the right and bottom image border, the
      usage convention is that the border area starts \em immediately beneath the
      image area on all four sides. Also note that you can access all image data
      in the border and the alignment area just like normal bitmap area.
      The total image size that you may access is AskTotalWidth() times
      AskTotalHeight(). Do not modify the more even if AskStride() is larger.
	
      A Bitmap is not intended as standard image storage (even for
      greyscale images). Instead, use the Image class, which adds more
      information to the Bitmaps like the used colorspace.
      
      BTW., this class could not be named Pixmap since this collides with X11.
  */
  template <class Pel> class Bitmap
  {
  public:
    Bitmap(); ///< Create an empty bitmap. No provider is attached.
    Bitmap(const Bitmap&); /**< Copy-constructor. Note that image content is not
			      copied. Both bitmaps will share the same memory. */
    /// Create a new bitmap in main memory.
    Bitmap(int w,int h,int border=0,int halign=1,int valign=1);
    /// Initialize a new bitmap based on the full data of the specified provider.
    Bitmap(BitmapProvider<Pel>*);
    /** Destructor. If this was the last reference to the bitmap provider
	it is destroyed. */
    ~Bitmap();

    /** Create a new bitmap. The old bitmap data is lost. If the new bitmap
	fits into the old bitmap memory space and it is not shared, the old
	space is reused. This is much faster than recreating a new bitmap. */
    void Create(int w,int h,int border=0,int halign=1,int valign=1);

    /** Detach bitmap content from the object. If this was the
	last handle to the bitmap data, the provider will be detroyed. */
    void Release();

    /** Attach the full data of a new provider. The provider may be NULL,
	which will result in a empty bitmap. */
    void AttachBitmapProvider(BitmapProvider<Pel>*);

    /// Make a copy of the specified bitmap by sharing the data.
    Bitmap<Pel> operator=(const Bitmap<Pel>&);

    /** Create a bitmap that shares only part of the current bitmap.
	The created bitmap will have no border and no alignment.
	If the bitmap is empty, an empty bitmap will be returned.
    */
    Bitmap<Pel> CreateSubView  (int x0,int y0,int w,int h) const;

    /** Create a bitmap that shares only a single field from the
	current (frame based) bitmap. In the created bitmap, the
	field lines are accessed using consecutive lines, instead of
	offsets of two. The border sizes an alignment sizes are
	adapted correspondingly.
	If the bitmap is empty, an empty bitmap will be returned.
    */
    Bitmap<Pel> CreateFieldView(bool top) const;

    void MoveZero(int x0,int y0);

    /** Copy the bitmap into a new, independent memory area. Alignments can be changed
	on the fly. If border<0, the old border size is used for the new bitmap. */
    Bitmap<Pel> Clone(int border=-1,int halign=1,int valign=1) const;

    int AskWidth()  const;         ///< Width of bitmap excluding border and alignments.
    int AskHeight() const;         ///< Height of bitmap excluding border and alignments.
    int AskAlignedWidth() const;   ///< Width of bitmap including alignments.
    int AskAlignedHeight() const;  ///< Height of bitmap including alignments.
    int AskTotalWidth() const;     ///< Width of bitmap including borders and alignments.
    int AskTotalHeight() const;    ///< Height of bitmap including borders and alignments.
    int AskBorder() const;         ///< Border width (as requested).
    int AskAlignedBorder() const;  ///< Actual border width including alignments.
    /** Bitmap stride (number of data elements to get from one line to the next).
	Note that this is not necessarily equal to AskTotalWidth() since a SubView or
	FieldView bitmap has additional data between its lines.
    */
    int AskStride() const { return AskFrame()[1]-AskFrame()[0]; }

    int getWidth() const { return AskWidth(); }
    int getHeight() const { return AskHeight(); }
    int getStride() const { return AskStride(); }
    Pel* getData() { return AskFrame()[0]; }
    const Pel* getData() const { return AskFrame()[0]; }
    typedef Pel PixelType;

    /* NOTE: use of Ask*Offset() is deprecated. Use Ask[Min/Max][X/Y] instead. */
    int AskXOffset() const { return d_xoffset; }
    int AskYOffset() const { return d_yoffset; }

    int AskMinX() const { return -d_xoffset; }
    int AskMinY() const { return -d_yoffset; }
    int AskMaxX() const { return -d_xoffset+d_width-1; }
    int AskMaxY() const { return -d_yoffset+d_height-1; }

    /// Check if bitmap has any data associated with it.
    bool IsEmpty() const { return d_parent==NULL; }

    /** Get a pointer-array to the actual bitmap data ([y][x]). Pixel [0][0] is
	the top-left pixel _inside_ the image. I.e. if the bitmap has a border
	around it, the border pixels can be access by negative positions or with
	x,y coordinates above AskWidth()/AskHeight(). This method comes
	additionally in a const flavour.
    */
    Pel*const* AskFrame();
    /** Const flavour of the non-const AskFrame(). Returns same value, but
	bitmap data may not be modified.
     */
    const Pel*const* AskFrame() const;

    const Pel* operator[](int y) const { return AskFrame()[y]; }
          Pel* operator[](int y)       { return AskFrame()[y]; }

    /// Return true iff the bitmap data is also used by another Bitmap object.
    bool IsS
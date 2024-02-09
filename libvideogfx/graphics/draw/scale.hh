/*********************************************************************
  libvideogfx/graphics/draw/scale.hh

  purpose:
    Functions for extending borders, change format of bitmaps...

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    04/Jul/2000 - Dirk Farin - bitmap format conversion and helpers
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

#ifndef LIBVIDEOGFX_GRAPHICS_DRAW_SCALE_HH
#define LIBVIDEOGFX_GRAPHICS_DRAW_SCALE_HH

#include <libvideogfx/graphics/datatypes/bitmap.hh>
#include <libvideogfx/graphics/datatypes/image.hh>

namespace videogfx {

  /* In Scale_NN and Scale_Bilinear, scaling is done such that the first pixels and
     the last pixels in each row/column match exactly. For an assignment that matches
     more closely to, e.g., the binomial decimation, a different scaling table would
     be needed (TODO / w/newW instead of (w-1)/(newW-1) ).
  */

  template <class Pel> void Scale_NN      (Bitmap<Pel>& dst,const Bitmap<Pel>& src, int newWidth, int newHeight);
  template <class Pel> void Scale_Bilinear(Bitmap<Pel>& dst,const Bitmap<Pel>& src, int newWidth, int newHeight);

  template <class Pel> void DoubleSize_Dup  (Bitmap<Pel>& dst,const Bitmap<Pel>& src);
  template <class Pel> void DoubleSize_Dup_H(Bitmap<Pel>& dst,const Bitmap<Pel>& src);
  template <class Pel> void DoubleSize_Dup_V(Bitmap<Pel>& dst,const Bitmap<Pel>& src);

  template <class Pel> void HalfSize_Avg  (Bitmap<Pel>& dst,const Bitmap<Pel>& src);
  template <class Pel> void HalfSize_Avg_H(Bitmap<Pel>& dst,const Bitmap<Pel>& src);
  template <class Pel> void HalfSize_Avg_V(Bitmap<Pel>& dst,const Bitmap<Pel>& src);

  template <class Pel> void ScaleDownOctaves_Avg(Bitmap<Pel>& dst,const Bitmap<Pel>& src, int nOctaves);


  /* Copy scaled version of region into another bitmap.
   */
  template <class Pel> void CopyScaled(Bitmap<Pel>& dst,       int dstx0,int dsty0, int dw,int dh,
				       const Bitmap<Pel>& src, int srcx0,int srcy0, int sw,int sh);
  template <class Pel> void CopyScaled(Image<Pel>& dst,       int dstx0,int dsty0, int dw,int dh,
				       const Image<Pel>& src, int srcx0,int srcy0, int sw,int sh);

  // Same as above, complete image/bitmap is inserted.
  template <class Pel> void CopyScaled(Bitmap<Pel>& dst,       int dstx0,int dsty0, int dw,int dh,
				       const Bitmap<Pel>& src);
  template <class Pel> void CopyScaled(Image<Pel>& dst,       int dstx0,int dsty0, int dw,int dh,
				       const Image<Pel>& src);


  // ------------------------------- implementation -----------------------------------

  template <class Pel> void DoubleSize_Dup  (Bitmap<Pel>& dst,const Bitmap<Pel>& src)
  {
    assert(&dst != &src);

    const int w = src.AskWidth();
    const int h = src.AskHeight();

    dst.Create(w*2,h*2);

    const Pel*const* sp = src.AskFrame();
    Pel*const* dp = dst.AskFrame();

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
	{
	  dp[y*2  ][x*2  ] =
	    dp[y*2  ][x*2+1] =
	    dp[y*2+1][x*2  ] =
	    dp[y*2+1][x*2+1] = sp[y][x];
	}
  }

  template <class Pel> void DoubleSize_Dup_H(Bitmap<Pel>& dst,const Bitmap<Pel>& src)
  {
    assert(&dst != &src);

    const int w = src.AskWidth();
    const int h = src.AskHeight();

    dst.Create(w*2,h);

    const Pel*const* sp = src.AskFrame();
    Pel*const* dp = dst.AskFrame();

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
	{
	  dp[y][x*2  ] =
	    dp[y][x*2+1] = sp[y][x];
	}
  }

  template <class Pel> void DoubleSize_Dup_V(Bitmap<Pel>& dst,const Bitmap<Pel>& src)
  {
    assert(&dst != &src);

    const int w = src.AskWidth();
    const int h = src.AskHeight();

    dst.Create(w,h*2);

    const Pel*const* sp = src.AskFrame();
    Pel*const* dp = dst.AskFrame();

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
	{
	  dp[y*2  ][x] =
	    dp[y*2+1][x] = sp[y][x];
	}
  }

  template <class Pel> void HalfSize_Avg  (Bitmap<Pel>& dst,const Bitmap<Pel>& src)
  {
    assert(&dst != &src);

    const int w = src.AskWidth();
    const int h = src.AskHeight();

    const int newh = (h+1)/2;
    const int neww = (w+1)/2;

    dst.Create(neww,newh);

    const Pel
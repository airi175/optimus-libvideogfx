/********************************************************************************
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

#include "libvideogfx/graphics/measure/snr.hh"

#include <math.h>
#include <algorithm>
using namespace std;

namespace videogfx {

  double CalcMSE(const Bitmap<Pixel>& img1,
		 const Bitmap<Pixel>& img2,
		 int x0,int y0,int x1,int y1)
  {
    const Pixel*const* p1 = img1.AskFrame();
    const Pixel*const* p2 = img2.AskFrame();

    if (x1<0) x1 = img1.AskWidth
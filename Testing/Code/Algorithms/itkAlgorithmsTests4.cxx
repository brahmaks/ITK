/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkAlgorithmsTests4.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "vnl/vnl_sample.h"
#include "itkTestMain.h" 


void RegisterTests()
{
  vnl_sample_reseed(8775070);
  REGISTER_TEST(itkNarrowBandCurvesLevelSetImageFilterTest );
  REGISTER_TEST(itkVectorThresholdSegmentationLevelSetImageFilterTest );
  REGISTER_TEST(itkWatershedImageFilterTest );
  REGISTER_TEST(itkVoronoiPartitioningImageFilterTest );
  REGISTER_TEST(itkVnlFFTTest);
#if defined(USE_FFTW)
  REGISTER_TEST(itkFFTWFFTTest);
#endif
#if defined(USE_SCSL)
  REGISTER_TEST(itkSCSLFFTTest);
#endif
}  


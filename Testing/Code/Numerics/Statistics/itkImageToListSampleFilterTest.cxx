/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkImageToListSampleFilterTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// The example tests the class itk::Statistics::ImageToListSampleFilter.
// The class is capable of generating an itk::ListSample from an image
// confined to a mask (if specified). This test exercises that.

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImageToListSampleFilter.h"
#include "itkImageRegionIteratorWithIndex.h"

typedef itk::Image< unsigned int , 2 > ImageType;
typedef itk::Image< unsigned char, 2 > MaskImageType;

//------------------------------------------------------------------------
// Creates a 10 x 10 image of unsigned chars with pixel at location
// (x,y) being yx. ie Pixel at (6,4) = 46.
//
static ImageType::Pointer CreateImage()
{
  ImageType::Pointer image = ImageType::New();

  ImageType::IndexType start;
  ImageType::SizeType  size;

  start.Fill( 0 );

  size[0] = 10;
  size[1] = 10;

  ImageType::RegionType region( start, size );
  image->SetRegions( region );
  image->Allocate();
  typedef itk::ImageRegionIteratorWithIndex< ImageType > IteratorType;
  IteratorType it( image, region );
  it.GoToBegin();
  while( !it.IsAtEnd() )
    {
    it.Set( it.GetIndex()[1] * 10 + it.GetIndex()[0]);
    ++it;
    }
  return image;
}

//------------------------------------------------------------------------
// Creates a 10 x 10 image of unsigned chars with pixel from (2,3) - (8,5) as
// 255 and rest as 0
static MaskImageType::Pointer CreateMaskImage()
{
  MaskImageType::Pointer image = MaskImageType::New();
  MaskImageType::IndexType start;
  MaskImageType::SizeType  size;

  start.Fill(0);
  size.Fill(10);

  MaskImageType::RegionType region( start, size );
  image->SetRegions( region );
  image->Allocate();
  image->FillBuffer(0);

  MaskImageType::IndexType startMask;
  MaskImageType::SizeType  sizeMask;

  startMask[0] = 2;
  startMask[1] = 3;

  sizeMask[0] = 7;
  sizeMask[1] = 3;

  MaskImageType::RegionType regionMask( startMask, sizeMask);
  typedef itk::ImageRegionIteratorWithIndex< MaskImageType > IteratorType;
  IteratorType it( image, regionMask );
  it.GoToBegin();
  while (!it.IsAtEnd())
    {
    it.Set((unsigned char)255);
    ++it;
    }
  return image;
}

//------------------------------------------------------------------------
// Creates a 13 x 17 image for testing verification of LargestPossibleRegion
static MaskImageType::Pointer CreateLargerMaskImage()
{
  MaskImageType::Pointer image = MaskImageType::New();

  MaskImageType::IndexType start;
  MaskImageType::SizeType  size;

  start[0] = 0;
  start[1] = 0;

  size[0] = 13;
  size[1] = 17;

  MaskImageType::RegionType region( start, size );
  image->SetRegions( region );
  image->Allocate();
  image->FillBuffer(0);
  return image;
}


int itkImageToListSampleFilterTest(int, char* [] )
{
  ImageType::Pointer     image     = CreateImage();
  MaskImageType::Pointer maskImage = CreateMaskImage();

  // Generate a list sample from "image" confined to the mask, "maskImage".
  typedef itk::Statistics::ImageToListSampleFilter<
    ImageType, MaskImageType > ImageToListSampleFilterType;
  ImageToListSampleFilterType::Pointer filter
                              = ImageToListSampleFilterType::New();

  bool pass = true;
  std::string failureMeassage="";

  //Invoke update before adding an input. An exception should be
  //thrown.
  try
    {
    filter->Update();
    failureMeassage = "Exception should have been thrown since \
                    Update() is invoked without setting an input ";
    pass = false;
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception caught: " << excp << std::endl;
    }
  // Restore the pipeline after the exception
  filter->ResetPipeline();

  if ( filter->GetInput() != NULL )
    {
    pass = false;
    failureMeassage = "GetInput() should return NULL if the input \
                     has not been set";
    }

  if ( filter->GetMaskImage() != NULL )
    {
    pass = false;
    failureMeassage = "GetMaskImage() should return NULL if mask image \
                     has not been set";
    }


  //generate list sample without a mask image
  filter->SetInput( image );
  filter->Update();


  //use a mask image
  filter->SetMaskImage( maskImage );
  filter->SetMaskValue( 255 );
  filter->Update();

  std::cout << filter->GetNameOfClass() << std::endl;
  filter->Print(std::cout);


  ImageToListSampleFilterType::MaskPixelType pixelType = filter->GetMaskValue();

  if( pixelType != 255 )
    {
    std::cerr << "Problem in SetMaskValue() GetMaskValue() " << std::endl;
    return EXIT_FAILURE;
    }

  typedef ImageToListSampleFilterType::ListSampleType ListSampleType;
  const ListSampleType * list = filter->GetOutput();

  // Check the sum of the pixels in the list sample. This should
  // be 945
  ListSampleType::ConstIterator lit = list->Begin();
  unsigned int sum = 0;
  while (lit != list->End())
    {
    sum += lit.GetMeasurementVector()[0];
    ++lit;
    }

  if (sum != 945)
    {
    pass = false;
    failureMeassage = "Wrong sum of pixels";
    std::cerr << "Computed sum of pixels in the list sample (masked) is : "
              << sum
              << " but should be 945.";
    }


  // Set on purpose a mask of inconsistent LargestPossibleRegion
  filter->SetMaskImage( CreateLargerMaskImage() );

  try
    {
    filter->Update();
    std::cerr << "Exception should have been thrown since \
      the mask has a different LargestPossibleRegion." << std::endl;
    return EXIT_FAILURE;
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Expected Exception caught: " << excp << std::endl;
    }

  if ( !pass )
    {
    std::cerr << "[FAILED]" << failureMeassage << std::endl;
    return EXIT_FAILURE;
    }

  std::cerr << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}
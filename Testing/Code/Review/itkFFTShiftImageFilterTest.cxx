/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkFFTShiftImageFilterTest.cxx
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

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkSimpleFilterWatcher.h"
#include <itkFFTShiftImageFilter.h>
#include "itkRGBPixel.h"

int itkFFTShiftImageFilterTest(int argc, char * argv[])
{

  if( argc != 4 )
    {
    std::cerr << "usage: " << argv[0] << " inputImage outputImage inverse" << std::endl;
    std::cerr << "  inputImage: The input image." << std::endl;
    std::cerr << "  outputImage: The output image." << std::endl;
    std::cerr << "  inverse: 0, to perform a forward transform, or 1 to perform" << std::endl;
    std::cerr << "           an inverse transform." << std::endl;
    exit(1);
    }

  const int dim = 3;
  
  typedef itk::RGBPixel< unsigned char > PType;
  typedef itk::Image< PType, dim >       IType;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  typedef itk::FFTShiftImageFilter< IType, IType > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );

  // test default values
  if ( filter->GetInverse( ) != false )
    {
    std::cerr << "Wrong default Inverse." << std::endl;
    return EXIT_FAILURE;
    }

  filter->SetInverse( atoi( argv[3] ) );
  if ( filter->GetInverse( ) != (bool)atoi(argv[3]) )
    {
    std::cerr << "Set/Get Inverse problem." << std::endl;
    return EXIT_FAILURE;
    }

  itk::SimpleFilterWatcher watcher(filter, "filter");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( argv[2] );

  try
    {
    writer->Update();
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;

}

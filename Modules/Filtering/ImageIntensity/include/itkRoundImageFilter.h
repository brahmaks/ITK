/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkRoundImageFilter_h
#define __itkRoundImageFilter_h

#include "itkUnaryFunctorImageFilter.h"
#include "itkMath.h"

namespace itk
{
namespace Functor
{
/**
 * \class Round
 * \brief
 * \ingroup ITKImageIntensity
 */
template< class TInput, class TOutput >
class Round
{
public:
  Round() {}
  ~Round() {}
  bool operator!=(const Round &) const
  {
    return false;
  }

  bool operator==(const Round & other) const
  {
    return !( *this != other );
  }

  inline TOutput operator()(const TInput & A) const
  {
    return itk::Math::Round<TOutput,TInput>( A );
  }
};
}
/** \class RoundImageFilter
 * \brief Rounds the value of each pixel.
 *
 * The computations are performed using itk::Math::Round(x).
 *
 * \ingroup IntensityImageFilters
 * \ingroup MultiThreaded
 * \ingroup ITKImageIntensity
 */
template< class TInputImage, class TOutputImage >
class RoundImageFilter:
  public
  UnaryFunctorImageFilter< TInputImage, TOutputImage,
                           Functor::Round< typename TInputImage::PixelType,
                                           typename TOutputImage::PixelType >   >
{
public:
  /** Standard class typedefs. */
  typedef RoundImageFilter Self;
  typedef UnaryFunctorImageFilter<
    TInputImage, TOutputImage,
    Functor::Round< typename TInputImage::PixelType,
                   typename TOutputImage::PixelType > >  Superclass;

  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(RoundImageFilter,
               UnaryFunctorImageFilter);

protected:
  RoundImageFilter() {}
  virtual ~RoundImageFilter() {}

private:
  RoundImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);  //purposely not implemented
};
} // end namespace itk

#endif

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
#ifndef __itkVolumeSplineKernelTransform_hxx
#define __itkVolumeSplineKernelTransform_hxx
#include "itkVolumeSplineKernelTransform.h"

namespace itk
{
template< class TScalar, unsigned int NDimensions >
void
VolumeSplineKernelTransform< TScalar, NDimensions >::ComputeG(const InputVectorType & x,
                                                                  GMatrixType & gmatrix) const
{
  const TScalar r = x.GetNorm();

  gmatrix.fill(NumericTraits< TScalar >::Zero);
  const TScalar r3 = r * r * r;
  for ( unsigned int i = 0; i < NDimensions; i++ )
    {
    gmatrix[i][i] = r3;
    }
}

template< class TScalar, unsigned int NDimensions >
void
VolumeSplineKernelTransform< TScalar, NDimensions >::ComputeDeformationContribution(
  const InputPointType  & thisPoint,
  OutputPointType & result) const
{
  unsigned long numberOfLandmarks =
    this->m_SourceLandmarks->GetNumberOfPoints();

  PointsIterator sp  = this->m_SourceLandmarks->GetPoints()->Begin();

  for ( unsigned int lnd = 0; lnd < numberOfLandmarks; lnd++ )
    {
    InputVectorType   position = thisPoint - sp->Value();
    const TScalar r = position.GetNorm();
    const TScalar r3 = r * r * r;

    for ( unsigned int odim = 0; odim < NDimensions; odim++ )
      {
      result[odim] += r3 * this->m_DMatrix(odim, lnd);
      }
    ++sp;
    }
}
} // namespace itk
#endif

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
#ifndef __itkCenteredEuler3DTransform_hxx
#define __itkCenteredEuler3DTransform_hxx

#include "itkCenteredEuler3DTransform.h"

namespace itk
{
// Constructor with default arguments
template <class TScalar>
CenteredEuler3DTransform<TScalar>::CenteredEuler3DTransform() :
  Superclass(ParametersDimension)
{
}

// Constructor with default arguments
template <class TScalar>
CenteredEuler3DTransform<TScalar>::CenteredEuler3DTransform(unsigned int parametersDimension) :
  Superclass(parametersDimension)
{
}

// Constructor with default arguments
template <class TScalar>
CenteredEuler3DTransform<TScalar>::CenteredEuler3DTransform(const MatrixType & matrix,
                                                                const OutputPointType & offset) :
  Superclass(matrix, offset)
{
}

// Destructor
template <class TScalar>
CenteredEuler3DTransform<TScalar>::
~CenteredEuler3DTransform()
{
}

//
// Set Parameters
//
// Parameters are ordered as:
//
// p[0:2] = rotations about x, y and z axes
// p[3:5} = center of rotation
// p[6:8] = translation
//
//
template <class TScalar>
void
CenteredEuler3DTransform<TScalar>
::SetParameters(const ParametersType & parameters)
{
  itkDebugMacro(<< "Setting parameters " << parameters);

  // Save parameters
  if( &parameters != &(this->m_Parameters) )
    {
    this->m_Parameters = parameters;
    }

  const ScalarType angleX = parameters[0];
  const ScalarType angleY = parameters[1];
  const ScalarType angleZ = parameters[2];
  this->SetVarRotation(angleX, angleY, angleZ);

  CenterType newCenter;
  typedef typename CenterType::ValueType CenterValueType;
  newCenter[0] = parameters[3];
  newCenter[1] = parameters[4];
  newCenter[2] = parameters[5];
  this->SetVarCenter(newCenter);
  this->ComputeMatrix();

  TranslationType newTranslation;
  newTranslation[0] = parameters[6];
  newTranslation[1] = parameters[7];
  newTranslation[2] = parameters[8];
  this->SetVarTranslation(newTranslation);
  this->ComputeOffset();

  // Modified is always called since we just have a pointer to the
  // parameters and cannot know if the parameters have changed.
  this->Modified();

  itkDebugMacro(<< "After setting parameters ");
}

//
// Get Parameters
//
// Parameters are ordered as:
//
// p[0:2] = rotations about x, y and z axes
// p[3:5} = center of rotation
// p[6:8] = translation
//

template <class TScalar>
const typename CenteredEuler3DTransform<TScalar>::ParametersType
& CenteredEuler3DTransform<TScalar>
::GetParameters(void) const
  {
  ParametersType parameters;

  this->m_Parameters[0] = this->GetAngleX();
  this->m_Parameters[1] = this->GetAngleY();
  this->m_Parameters[2] = this->GetAngleZ();

  this->m_Parameters[3] = this->GetCenter()[0];
  this->m_Parameters[4] = this->GetCenter()[1];
  this->m_Parameters[5] = this->GetCenter()[2];

  this->m_Parameters[6] = this->GetTranslation()[0];
  this->m_Parameters[7] = this->GetTranslation()[1];
  this->m_Parameters[8] = this->GetTranslation()[2];

  return this->m_Parameters;
  }

template <class TScalar>
void
CenteredEuler3DTransform<TScalar>
::ComputeJacobianWithRespectToParameters(const InputPointType & p, JacobianType & jacobian) const
{
  // need to check if angles are in the right order
  const double cx = vcl_cos( this->GetAngleX() );
  const double sx = vcl_sin( this->GetAngleX() );
  const double cy = vcl_cos( this->GetAngleY() );
  const double sy = vcl_sin( this->GetAngleY() );
  const double cz = vcl_cos( this->GetAngleZ() );
  const double sz = vcl_sin( this->GetAngleZ() );

  jacobian.SetSize( 3, this->GetNumberOfLocalParameters() );
  jacobian.Fill(0.0);

  const double px = p[0] - this->GetCenter()[0];
  const double py = p[1] - this->GetCenter()[1];
  const double pz = p[2] - this->GetCenter()[2];

  if( this->GetComputeZYX() )
    {
    jacobian[0][0] = ( cz * sy * cx + sz * sx ) * py + ( -cz * sy * sx + sz * cx ) * pz;
    jacobian[1][0] = ( sz * sy * cx - cz * sx ) * py + ( -sz * sy * sx - cz * cx ) * pz;
    jacobian[2][0] = ( cy * cx ) * py + ( -cy * sx ) * pz;

    jacobian[0][1] = ( -cz * sy ) * px + ( cz * cy * sx ) * py + ( cz * cy * cx ) * pz;
    jacobian[1][1] = ( -sz * sy ) * px + ( sz * cy * sx ) * py + ( sz * cy * cx ) * pz;
    jacobian[2][1] = ( -cy ) * px + ( -sy * sx ) * py + ( -sy * cx ) * pz;

    jacobian[0][2] = ( -sz * cy ) * px + ( -sz * sy * sx - cz * cx ) * py
      + ( -sz * sy * cx + cz * sx ) * pz;
    jacobian[1][2] = ( cz * cy ) * px + ( cz * sy * sx - sz * cx ) * py
      + ( cz * sy * cx + sz * sx ) * pz;
    jacobian[2][2] = 0;
    }
  else
    {
    jacobian[0][0] = ( -sz * cx * sy ) * px + ( sz * sx ) * py + ( sz * cx * cy ) * pz;
    jacobian[1][0] = ( cz * cx * sy ) * px + ( -cz * sx ) * py + ( -cz * cx * cy ) * pz;
    jacobian[2][0] = ( sx * sy ) * px + ( cx ) * py + ( -sx * cy ) * pz;

    jacobian[0][1] = ( -cz * sy - sz * sx * cy ) * px + ( cz * cy - sz * sx * sy ) * pz;
    jacobian[1][1] = ( -sz * sy + cz * sx * cy ) * px + ( sz * cy + cz * sx * sy ) * pz;
    jacobian[2][1] = ( -cx * cy ) * px + ( -cx * sy ) * pz;

    jacobian[0][2] = ( -sz * cy - cz * sx * sy ) * px + ( -cz * cx ) * py
      + ( -sz * sy + cz * sx * cy ) * pz;
    jacobian[1][2] = ( cz * cy - sz * sx * sy ) * px + ( -sz * cx ) * py
      + ( cz * sy + sz * sx * cy ) * pz;
    jacobian[2][2] = 0;
    }

  // compute derivatives for the center of rotation part
  unsigned int blockOffset = 3;
  for( unsigned int dim = 0; dim < SpaceDimension; dim++ )
    {
    jacobian[dim][blockOffset + dim] = 1.0;
    }
  blockOffset += SpaceDimension;
  // compute derivatives for the translation part
  for( unsigned int dim = 0; dim < SpaceDimension; dim++ )
    {
    jacobian[dim][blockOffset + dim] = 1.0;
    }
}

// Get an inverse of this transform
template <class TScalar>
bool
CenteredEuler3DTransform<TScalar>
::GetInverse(Self *inverse) const
{
  return this->Superclass::GetInverse(inverse);
}

// Return an inverse of this transform
template <class TScalar>
typename CenteredEuler3DTransform<TScalar>::InverseTransformBasePointer
CenteredEuler3DTransform<TScalar>
::GetInverseTransform() const
{
  Pointer inv = New();

  return this->GetInverse(inv) ? inv.GetPointer() : NULL;
}

// Print self
template <class TScalar>
void
CenteredEuler3DTransform<TScalar>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

} // namespace

#endif

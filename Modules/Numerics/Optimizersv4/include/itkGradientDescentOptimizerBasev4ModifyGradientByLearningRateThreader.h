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
#ifndef __itkGradientDescentOptimizerBasev4ModifyGradientByLearningRateThreader_h
#define __itkGradientDescentOptimizerBasev4ModifyGradientByLearningRateThreader_h

#include "itkDomainThreader.h"
#include "itkThreadedIndexedContainerPartitioner.h"

namespace itk
{

template<class TInternalComputationValueType>
class GradientDescentOptimizerBasev4Template;

/** \class GradientDescentOptimizerBasev4ModifyGradientByLearningRateThreaderTemplate
 * \brief Modify the gradient by the learning rate for
 * GradientDescentOptimizerBasev4.
 * \ingroup ITKOptimizersv4
 */

template<class TInternalComputationValueType>
class GradientDescentOptimizerBasev4ModifyGradientByLearningRateThreaderTemplate
  : public DomainThreader< ThreadedIndexedContainerPartitioner, GradientDescentOptimizerBasev4Template<TInternalComputationValueType> >
{
public:
  /** Standard class typedefs. */
  typedef GradientDescentOptimizerBasev4ModifyGradientByLearningRateThreaderTemplate              Self;
  typedef DomainThreader< ThreadedIndexedContainerPartitioner,
                          GradientDescentOptimizerBasev4Template<TInternalComputationValueType> >  Superclass;
  typedef SmartPointer< Self >                                                                    Pointer;
  typedef SmartPointer< const Self >                                                              ConstPointer;

  itkTypeMacro( GradientDescentOptimizerBasev4ModifyGradientByLearningRateThreaderTemplate, DomainThreader );

  itkNewMacro( Self );

  typedef typename Superclass::DomainType             DomainType;
  typedef typename Superclass::AssociateType          AssociateType;
  typedef DomainType                                  IndexRangeType;

protected:
  virtual void ThreadedExecution( const IndexRangeType & subrange,
                                  const ThreadIdType threadId );

  GradientDescentOptimizerBasev4ModifyGradientByLearningRateThreaderTemplate() {}
  virtual ~GradientDescentOptimizerBasev4ModifyGradientByLearningRateThreaderTemplate() {}

private:
  GradientDescentOptimizerBasev4ModifyGradientByLearningRateThreaderTemplate( const Self & ); // purposely not implemented
  void operator=( const Self & ); // purposely not implemented
};

/** This helps to meet backward compatibility */
typedef GradientDescentOptimizerBasev4ModifyGradientByLearningRateThreaderTemplate<double> GradientDescentOptimizerBasev4ModifyGradientByLearningRateThreader;

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkGradientDescentOptimizerBasev4ModifyGradientByLearningRateThreader.hxx"
#endif

#endif

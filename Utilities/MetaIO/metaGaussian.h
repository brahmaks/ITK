#include "metaTypes.h"

#define NAMESPACE_METAGAUSSIAN_H META_MERGE_TOKENS($METAIO_NAMESPACE, \
                                                 METAGAUSSIAN_H)
#ifndef $NAMESPACE_METAGAUSSIAN_H
#define $NAMESPACE_METAGAUSSIAN_H

#include "metaUtils.h"
#include "metaObject.h"

#include <list>


/*!    MetaGaussian (.h and .cpp)
 *
 * Description:
 *    Reads and Writes MetaGaussianFiles.
 *
 * \author Mark Foskey
 * 
 * \date February 12, 2004
 * 
 * Depends on:
 *    MetaUtils.h
 *    MetaObject.h
 */

#if (METAIO_USE_NAMESPACE)
namespace METAIO_NAMESPACE {
#endif

class METAIO_EXPORT MetaGaussian : public MetaObject
  {

  /////
  //
  // PUBLIC
  //
  ////
  public:

    ////
    //
    // Constructors & Destructor
    //
    ////
    MetaGaussian();

    MetaGaussian(const char *_headerName);   

    MetaGaussian(const MetaGaussian *_gaussian);    

    MetaGaussian(unsigned int dim);

    ~MetaGaussian(void);

    void PrintInfo(void) const;

    void CopyInfo(const MetaGaussian * _gaussian);

    void  Clear(void);

    void Maximum(float val) { m_Maximum = val; }
    float Maximum() const { return m_Maximum; }

    void Radius(float val) { m_Radius = val; }
    float Radius() const { return m_Radius; }

  ////
  //
  // PROTECTED
  //
  ////
  protected:

    void  M_Destroy(void);

    void  M_SetupReadFields(void);

    void  M_SetupWriteFields(void);

    bool  M_Read(void);

    float m_Maximum;

    float m_Radius;

  };


#if (METAIO_USE_NAMESPACE)
};
#endif


#endif

/*-* mode:C++;tab-with=8;c-basic-offset:8 -*- */
/* zhendong wan */
#ifndef __NULLMATRIX_H
#define __NULLMATRIX_H

#include <linbox/blackbox/archetype.h>
#include <linbox/util/debug.h>
 
namespace LinBox{
  
  /// is a Blackbox class supporting a single instance of the 0 by 0 matrix.
  template<class Vector>
    class NullMatrix : public BlackboxArchetype<Vector> {
    protected:
    static NullMatrix* nullMatrix;
    public:
    NullMatrix() {cout << "NullMatrix default cstor" << endl;}
    NullMatrix(const NullMatrix& n) {}

    virtual BlackboxArchetype<Vector>* clone() const {return new NullMatrix(getInstance());}

    static NullMatrix& getInstance() { 
      if(nullMatrix==0) 
	nullMatrix=new NullMatrix();

      return *nullMatrix;
    }

    virtual Vector& apply(Vector& y, const Vector& x) const {
      linbox_check(y.size()==0);
      linbox_check(x.size()==0);
      return y;
    }

    /* applyIn is depreciated.  If you have a desire to use it, please tell me about that.  -bds
    virtual Vector& applyIn(Vector& x) const {
      linbox_check(x.size()==0);
      return x;
    }
    */

    virtual Vector& applyTranspose(Vector& y, const Vector& x) const {
      linbox_check(y.size()==0);
      linbox_check(x.size()==0);
      return y;
    }

    /* applyIn is depreciated.  If you have a desire to use it, please tell me about that.  -bds
    virtual Vector& applyTransposeIn(Vector& x) const {
      linbox_check(x.size()==0);
      return x;
    }
    */

    virtual inline size_t rowdim() const {return 0;}

    virtual inline size_t coldim() const {return 0;}
    protected:
    NullMatrix() {}
  };

  template<class Vector>
    NullMatrix<Vector>* NullMatrix<Vector>::nullMatrix=0;
}

#endif

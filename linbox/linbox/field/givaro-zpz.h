/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
/* linbox/field/givaro-zpz.h
 * Copyright (C) 2002 Pascal Giorgi
 *
 * Written by Pascal Giorgi <pascal.giorgi@ens-lyon.fr>
 *
 * ------------------------------------
 *
 * See COPYING for license information.
 */

/* WARNING this wrapper works only with an improved version of Givaro.
 * This version of givaro won't be available for public yet.
 * But it is available on my web page.
 * You can send me a mail to get it or for others details.
 */

#ifndef __LINBOX_field_givaro_zpz_H
#define __LINBOX_field_givaro_zpz_H


#include "linbox/linbox-config.h"
#include "linbox/integer.h"
#include "linbox/field/field-interface.h"
#include "linbox/util/debug.h"
#include "linbox/vector/vector-domain.h"
//-------------------------------------
// Files of Givaro library
#include <givaro/givzpz.h>
#include <givaro/giv_randiter.h>
#include <linbox/field/field-traits.h>

//--------------------------------------

// Namespace in which all LinBox code resides
namespace LinBox
{

	/*  This wrappers allows to use three sorts of givaro fields :
	 *  Elements represent by a 32 bits integer
	 *  Elements represent by a 16 bits integer
	 *  Elements represent in Zech log representation by a 16 bits integer
	 *
	 *  To use this fields with the wrapper below just replace the template
	 *  parameter by the Tag appropriated.
	 *  "Std16"  for 16 bits integer
	 *  "Std32"  for 32 bits integer
	 *  "Log16"  for Zech log representation in 16 bits
	 */
	template<class Field>
	class DotProductDomain;
	template<class Field>
	class FieldAXPY;

	template <class Ring>
	struct ClassifyRing;

	template <class TAG>
	class GivaroZpz;

	template<class Tag>
	struct ClassifyRing<GivaroZpz<Tag> > {
		typedef RingCategories::ModularTag categoryTag;
	};

	/** \brief wrapper of Givaro's ZpzDom.
	  \ingroup field

	 *  Most methods are inherited from ZpzDom<Std16>, ZpzDom<Std32>
	 *  and ZpzDom<log16> classes of Givaro.
	 *  These classes allow to construct only finite field with a prime modulus.
	 */

	template <class TAG> class GivaroZpz : public ZpzDom<TAG>, public FieldInterface
	{

	private:

		/*		friend class DotProductDomain<GivaroZpz<TAG> > ;
				friend class FieldAXPY<GivaroZpz<TAG> >; */

	public:

		//typedef integer Integer;

		/** Element type.
		 *  This type is inherited from the Givaro class ZpzDom<TAG>
		 */
		typedef typename ZpzDom<TAG>::Rep Element;

		/** RandIter type
		 *  This type is inherited from the Givaro class ZpzDom<TAG>
		 */
		typedef GIV_randIter< ZpzDom<TAG>, integer > RandIter;

		/** Constructor from an integer
		 *  this constructor use the ZpzDom<TAG> constructor
		 */
		GivaroZpz (const integer &p) :
			ZpzDom<TAG> (static_cast<typename ZpzDom<TAG>::Residu_t> (p))
		{}


		/** Constructor from an integer (takes degree of extension as 2nd parameter, must be 1)
		 *  this constructor use the ZpzDom<TAG> constructor
		 */
		GivaroZpz (const integer &p, const integer& k) :
			ZpzDom<TAG> (static_cast<typename ZpzDom<TAG>::Residu_t> (p))
		{

			if (k!=1)
				throw PreconditionFailed(__func__,__FILE__,__LINE__,"exponent must be 1");
		}

		/** Copy constructor.
		 * This copy constructor use the ZpzDom<TAG> copy constructor
		 */
		GivaroZpz (const GivaroZpz<TAG>& F) :
			ZpzDom<TAG> (F)
		{}


		// Rich Seagraves 7-16-2003
		// As is, this operator is an infinite loop
		// By not providing an operator= in GivaroZpz,
		// the operator= in the base class (ZpzDom<TAG>) is called
		// automatically by the rules of C++, which I'm guessing is
		// the "Right Thing" for this operator
		//
		/** Operator =
		*/
		/*
		   GivaroZpz<TAG>& operator= (const GivaroZpz<TAG>& F) {
		   return (*this)=F;
		   }
		   */

		/** Characteristic.
		 * Return integer representing characteristic of the domain.
		 * @return integer representing characteristic of the domain.
		 */
		integer &characteristic (integer &c) const
		{ return c = integer (ZpzDom<TAG>::size ()); }
		long characteristic() const
		{return static_cast<int>(ZpzDom<TAG>::size());}

		/** Cardinality.
		 * Return integer representing cardinality of the domain.
		 * @return integer representing cardinality of the domain
		 */
		integer &cardinality (integer &c) const
		{ return c = integer (ZpzDom<TAG>::size ()); }

		/** Conversion of field base element to an integer.
		 * This function assumes the output field base element x has already been
		 * constructed, but that it is not already initialized.
		 * @return reference to an integer.
		 * @param x integer to contain output (reference returned).
		 * @param y constant field base element.
		 */
		integer &convert (integer &x, const Element &y) const
		{ return x = integer (y); }

		double &convert (double& x, const Element& y) const
		{ return x = static_cast<double>(y); }

		template<class Type>
		Type &convert (Type& x, const Element& y) const
		{ return x = static_cast<Type>(y); }


		/** Initialization of field base element from an integer.
		 * Behaves like C++ allocator construct.
		 * This function assumes the output field base element x has already been
		 * constructed, but that it is not already initialized.
		 * @return reference to field base element.
		 * @param x field base element to contain output (reference returned).
		 * @param y integer.
		 */
		Element &init (Element &x , const integer &y = 0) const
		{
			//
			//	AU 28/03/07 no cast to long allows to use ZpzDom<integer>
			//
			//ZpzDom<TAG>::init (x, (long) (y% integer(this->_p)));
			ZpzDom<TAG>::init (x, (y% integer(this->_p)));
			return x;
		}


		Element &init (Element &x , const double &y ) const
		{
			double z = fmod(y, (double) this->_p);
			if (z < 0) z += (double) this->_p;
			z += 0.5;
			return x = static_cast<long>(z); //rounds towards 0
		}

		static uint64_t getMaxModulus();

	}; // class GivaroZpz<TAG>


	template <> uint64_t GivaroZpz<Std32>::getMaxModulus() { return 46339; } // 2^15.5-1
	template <> uint64_t GivaroZpz<Std64>::getMaxModulus() { return 3037000499ULL; } // 2^15.5-1
	template <> uint64_t GivaroZpz<Unsigned32>::getMaxModulus() { return 65535; } // 2^16-1
	template <> uint64_t GivaroZpz<Std16>::getMaxModulus() { return 255; }   // 2^8-1
	template <> uint64_t GivaroZpz<Log16>::getMaxModulus() { return 32767; } // 2^15 - 1

	/** Specialisation of the convert function for the zech log representation
	 *	of givaro-zpz (GivaroZpz<Log16>.
	 *  this function translates the internal representation to the real
	 *	value of the element.
	 *	This can have no sense but can be usefull
	 *  NB : the init function for this specialisation does the same thing.
	 *  the function transaltes the values to her internal representation.
	 */
	template <> integer& GivaroZpz<Log16>::convert(integer& x, const Element& y) const
	{
		if (y>=this->_p) return x = 0;
		int tmp = _tab_rep2value[y];
		return x = integer (tmp);
	}

	template <> double& GivaroZpz<Log16>::convert(double& x, const Element& y) const
	{
		if (y>=this->_p) return x = 0.0;
		int tmp = _tab_rep2value[y];
		return x = (double) tmp;
	}

	template <> GivaroZpz<Log16>::Element& GivaroZpz<Log16>::init(GivaroZpz<Log16>::Element& x, const double& y) const
	{
		double z = fmod(y, (double) this->_p);
		if (z < 0) z += this->_p;
		z += 0.5;
		return x = _tab_value2rep[static_cast<long>(z)]; //rounds towards 0
	}

	template <> GivaroZpz<Log16>::Element& GivaroZpz<Log16>::init(GivaroZpz<Log16>::Element& x, const integer& y) const
	{
		int tmp =(int) (y % (integer)this->_p);
		if (tmp < 0 ) tmp += this->_p;
		return x = _tab_value2rep[tmp];
	}

	/* Specialization of FieldAXPY for GivaroZpz<Std32> Field */


	template <>
	class FieldAXPY<GivaroZpz<Std32> > {
	public:

		typedef GivaroZpz<Std32>::Element Element;
		typedef GivaroZpz<Std32> Field;

		FieldAXPY (const Field &F) :
			_F (F) , Corr(uint64_t(-1) % (uint64_t)F.characteristic() +1)
		{ _y = 0; }
		FieldAXPY (const FieldAXPY &faxpy) :
			_F (faxpy._F), _y (0) , Corr(faxpy.Corr)
		{}

		FieldAXPY<GivaroZpz<Std32> > &operator = (const FieldAXPY &faxpy)
		{ _F = faxpy._F; _y = faxpy._y; Corr = faxpy.Corr; return *this; }

		inline uint64_t& mulacc (const Element &a, const Element &x)
		{
			uint64_t t = (uint64_t) a * (uint64_t) x;
			_y += t;
			if (_y < t)
				return _y += Corr;
			else
				return _y;
		}

		inline uint64_t& accumulate (const Element &t)
		{
			_y += t;
			if (_y < (uint64_t)t)
				return _y += Corr;
			else
				return _y;
		}

		inline Element &get (Element &y) {
			_y %= (uint64_t) _F.characteristic();
			if ((int64_t) _y < 0) _y += _F.characteristic();
			y = (uint32_t) _y;
			return y;
		}

		inline FieldAXPY &assign (const Element y)
		{ _y = y; return *this; }

		inline void reset() {
			_y = 0;
		}

	private:

		Field _F;
		uint64_t _y;
		uint64_t Corr;
	};




	/* Specialization of FieldAXPY for GivaroZpz<Std32> Field */

	template <>
	class FieldAXPY<GivaroZpz<Std16> > {
	public:

		typedef GivaroZpz<Std16>::Element Element;
		typedef GivaroZpz<Std16> Field;

		FieldAXPY (const Field &F) :
			_F (F) , Corr(uint32_t(-1) % (uint32_t)F.characteristic() +1)
		{ _y = 0; }
		FieldAXPY (const FieldAXPY &faxpy) :
			_F (faxpy._F), _y (0) , Corr(faxpy.Corr)
		{}

		FieldAXPY<GivaroZpz<Std16> > &operator = (const FieldAXPY &faxpy)
		{ _F = faxpy._F; _y = faxpy._y; Corr = faxpy.Corr; return *this; }

		inline uint32_t& mulacc (const Element &a, const Element &x)
		{
			uint32_t t = (uint32_t) a * (uint32_t) x;
			_y += t;

			if (_y < t)
				return _y += Corr;
			else
				return _y;
		}

		inline uint32_t& accumulate (const Element &t)
		{
			_y += t;

			if (_y < (uint32_t)t)
				return _y += Corr;
			else
				return _y;
		}

		inline Element &get (Element &y) {
			_y %= (uint32_t) _F.characteristic();
			if ((int32_t) _y < 0) _y += _F.characteristic();
			y = (uint16_t) _y;
			return y;
		}

		inline FieldAXPY &assign (const Element y)
		{ _y = y; return *this; }

		inline void reset() {
			_y = 0;
		}

	private:

		Field _F;
		uint32_t _y;
		uint32_t Corr;
	};



	// Specialization of DotProductDomain for GivaroZpz<Std32> field

	template <>
	class DotProductDomain<GivaroZpz<Std32> > :  private virtual VectorDomainBase<GivaroZpz<Std32> > {

	public:

		typedef GivaroZpz<Std32>::Element Element;

		DotProductDomain (const GivaroZpz<Std32> &F) :
			VectorDomainBase<GivaroZpz<Std32> > (F) ,
			Corr(uint64_t(-1) % (uint64_t)F.characteristic() +1),
			Max(uint64_t(-1))
		{}

	protected:
		template <class Vector1, class Vector2>
		inline Element &dotSpecializedDD (Element &res, const Vector1 &v1, const Vector2 &v2) const;

		template <class Vector1, class Vector2>
		inline Element &dotSpecializedDSP (Element &res, const Vector1 &v1, const Vector2 &v2) const;

	private:
		uint64_t Corr;
		uint64_t Max;
	};

	// Specialization of DotProductDomain for GivaroZpz<Std16> field

	template <>
	class DotProductDomain<GivaroZpz<Std16> > :  private virtual VectorDomainBase<GivaroZpz<Std16> > {

	public:

		typedef GivaroZpz<Std16>::Element Element;

		DotProductDomain (const GivaroZpz<Std16> &F) :
			VectorDomainBase<GivaroZpz<Std16> > (F) ,
			Corr(uint32_t(-1) % (uint32_t)F.characteristic() +1),
			Max(uint32_t(-1))
		{}

	protected:
		template <class Vector1, class Vector2>
		inline Element &dotSpecializedDD (Element &res, const Vector1 &v1, const Vector2 &v2) const;

		template <class Vector1, class Vector2>
		inline Element &dotSpecializedDSP (Element &res, const Vector1 &v1, const Vector2 &v2) const;

	private:
		uint32_t Corr;
		uint32_t Max;
	};

} // namespace LinBox

#include "linbox/field/givaro-zpz.inl"

#endif // __LINBOX_field_givaro_zpz_H


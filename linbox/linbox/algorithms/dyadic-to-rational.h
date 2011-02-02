#ifndef __DYADICTORATIONAL_H
#define __DYADICTORATIONAL_H
/* dyadic-to-rational.h
 *
 * dyadicToRational reconstructs a rational a/b from dyadic approximation n/2^k.
 *
 * It is used in rational-solver-sn
 *
 * "Rational reconstruction" starts from a p-adic approximation.
 * This is different though very similar
 *
 *  Evolved by bds from rational-reconstruction2.h by Z. Wan.
 *
 *  standard linbox copyright/license applies.  See COPYING.
 */

#include <stack>
#include <assert.h>

#include <linbox/integer.h>
#include <linbox/util/commentator.h>

namespace LinBox{

/** Rational reconstruction of a/b from n/d with denominator bound B.
 * We give a/b, the continued fraction approximant of n/d that
 * satisfies |a/b - n/d| < 1/2d (well approximated) and 0 < b <= B.
 * Return value is 0, if no such approximant exists.
 * Return value is 1, if either
 *   (i) a second well approximated rational with denominator bounded by B may exist, or
 *   (ii) the well approximated condition is not met for a/b.
 *   In these cases, a/b may be used speculatively.
 * Return value is 2, if the approximant is guaranteed (because bB <= d).

 * If no fraction is well approximated the last b <= B in the remainder sequence of n,d is given.
 *
 * If d = 2^k and n = sum_i=l to k n_i 2^i, then * n/d = sum_{i=l down to 0} n_i/2^{k-i}
 * is a {\em dyadic rational}.  Numbers of this form are produced for example by
 * numeric-symbolic iterations.
 *
 * If it is known that n/d is the most accurate approximation with denominator d
 * to a/b, and that the denominator b is bounded by B, i.e. b <= B, then such a/b is
 * uniquely determined, provided d >= bB.
 * ...in that case, such a/b is returned by dyadicToRational().
 * This follows from two facts:
 * First, by definition, n/d is an accurate approximation to a/b
 * with b <= d when |n/d - a/b| < 1/2d.
 * Otherwise (n-1)/d or (n+1)/d would be a better approximation.
 * Second, if a/b and a'/b' are distinct rationals, then |a/b - a'/b'| >= 1/bb'.
 * Thus if a'/b' is another rational accurately approximated by n/d,
 * we have 1/bb' <= |a/b - a'/b'| <= |a/b - n/d| + |n/d - a'/b'| <= 1/2d + 1/2d = 1/d.
 * So bb' > d >= bB, thus b' > B.
 *
 * In summary: If it exists, the unique a/b is given such that n/d approximates a/b
 * to within 1/2d and b <= B.  Otherwise a plausible a/b is given or failure is signaled.
 *
 * "Symbolic-Numeric Exact Rational Linear System Solver" by Saunders, Wood, Youse.
 * describes the construction.
 */
template<class Ring>
int dyadicToRational (
	const Ring& Z,
	typename Ring::Element& a, typename Ring::Element& b,
	const typename Ring::Element& n, const typename Ring::Element& d,
	const typename Ring::Element& B)
{
	typedef typename Ring::Element Int;
	Int e; Z.init(e);// error term

	Int an; Z.init(an); Z.abs(an, n/*q*/);

	// Partial_hegcd is defined below.
	bool found = partial_hegcd(Z, e, b, an, d/*q*/, B); // e = b*an - ?*d and |b| <= B
	a = (e - b*an)/d; //div is exact
	// now a/b is solution but signs may be wrong
	Z.abs(b,b);
	Z.abs(a,a);
	Int zero; Z.init(zero, 0);
	if (Z.compare(n, zero) < 0)  Z.negin(a); // a = -a;

	bool guarantee = b*B < d;
	if ((!found && b > 0) || (found && ! guarantee)) return 1;
	if (found && guarantee) return 2;
	return 0; // b == 0
}

/** partial_hegcd() sets e, b from the remainder sequence of n,d.
 * It requires positive n and d.
 * It sets e to the first r_i (remainder) and
 * b to the corresponding q_i (coefficient of n)
 * such that 2r_i < |q_i| and |q_i| <= B (the given denominator bound).
 * True is returned iff such e, b exist.
 *
 * If not, b is the largest q_i such that |q_i| <= B,
 * and e is the corresponding remainder.  In this case b is the denominator
 * of a plausibly approximated but not well approximated rational.
 * It can be used speculatively.
 */
// later reintroduce swapping trick
template<class Ring>
bool partial_hegcd(Ring& Z, typename Ring::Element& e, typename Ring::Element& b, const typename Ring::Element& n, const typename Ring::Element& d, const typename Ring::Element& denBound){
	typedef typename Ring::Element integer;
	integer quo, r, tmp;  Z.init(quo); Z.init(r); Z.init(tmp);
	bool withinbound, wellapproximated;

	integer b0; Z.init(b0, 1); // and a0 = -0
	integer r0; Z.init(r0, n); // so that r0 = b0*n - a0*d
	integer b1; Z.init(b1, -0); // and a1 = 1
	integer r1; Z.init(r1, d); // so that r1 = b1*n - a1*d

	do {
		Z.quoRem(quo, e, r0, r1);
		//integer::divmod (quo, e, r0, r1);
		b = b0;
		Z.axmyin(b, quo, b1);// b = b0 - quo*b1;
		r0 = r1; b0 = b1;
		r1 = e; b1 = b;
		//assert(r1 >= 0);
		Z.abs(tmp, b1);
		withinbound = (Z.compare(tmp, denBound) <= 0);
	    wellapproximated = (Z.compare(2*r1 , tmp) <= 0);
	}
	while ( ! wellapproximated && withinbound );
	if (! withinbound) {e = r0; b = b0;} // make available for speculation
	return withinbound;

} // partial_hegcd

// vector rational reconstruction building num, den from numx, denx
template<class Ring>
int dyadicToRational(
	const Ring& Z,
	std::vector<typename Ring::Element>& num, typename Ring::Element& den,
	std::vector<typename Ring::Element>& numx, typename Ring::Element& denx,
	typename Ring::Element& denBound)
{
	typedef typename Ring::Element Int;
	Int q, rem, tmp_den, nx;
	Z.init(q); Z.init(rem); Z.init(tmp_den); Z.init(nx);
	Int one; Z.init(one, 1);
	Int two; Z.init(two, 2);
	Int denx2;
	Z.init(denx2, denx); Z.divin(denx2, two);// denx2 = denx/2, for balancing remainders.
	std::stack<std::pair<size_t, Int> > S;
	Int tmp; Z.init(tmp);

	Int den_lcm; Z.init(den_lcm, 1);
	den = den_lcm; // = 1.

	S.push(std::pair<int, Int>(0, 1));
	Int e; Z.init(e);// e for error
	int ret = 2; // 2 means guaranteed, 1 possible, 0 fail.
	for (size_t i = 0; i < num.size(); ++i) {
		Z.abs(nx, numx[i]);
		Z.mul(tmp, nx, den);
		Z.quoRem(num[i], e, tmp, denx); //nx*den - num[i]*denx = e, with num[i] and e nonneg.
		// we need |nx/denx - num[i]/den| == e/den*denx <= 1/2denx, so 2e <= den.
		// adjust to balanced remainder e.
		if (Z.compare(e, denx2) >= 0) {Z.subin(e, denx), Z.addin(num[i], one); }
		//nx*den = num[i]*denx + e , thus |nx/denx - num[i]/den| = e/denx*den

	// can try e < den && 2*e < den for speed
		Z.mul(tmp, two, Z.abs(e));
		if ( Z.compare(tmp, den) > 0)// 2|e| > den
		{   // we failed, so need another reconstruction
			int oneret = dyadicToRational (Z, tmp, tmp_den, nx, denx, denBound);
			if (oneret == 1) ret = 1;
			if ( oneret == 0 ) return oneret;
			//std::cerr << i << " tmp " << tmp << " num[i] " << num[i] << std::endl;
			num[i] = tmp;

			Z.lcm (den_lcm, tmp_den, den);
			Z.div( tmp, den_lcm, tmp_den ); // exact
			Z.mulin( num[i], tmp ); // num[i]/den_lcm = prev num[i]/tmp_den.

			Z.div(tmp, den_lcm, den); // exact
			// must multiply all prior num[i] by this tmp.
			S.push(std::pair<size_t, Int>(i, tmp));
			den = den_lcm;
			//assert(Z.compare(den, denBound)<=0);
			if (Z.compare(den, denBound)>0) return false; // den > denBound
		}

		Int zero; Z.init(zero);
		if (Z.compare(numx[i], zero) < 0) Z.negin(num[i]); // numx[i] < 0

	}
	// now fix shorties
	Int t; Z.init(t, 1);
	while ( S.size() > 1 ) {
		Z.mulin(t, S.top().second);
		int k = S.top().first; S.pop();
		int j = S.top().first;
		for (int i = k-1; i >= j; --i) {
			Z.mulin(num[i], t);
		}
	}
	S.pop();
	return ret;

} // vector dyadicToRational

/* ****
// vector rational reconstruction building num, den from numx, denx
// This one -- very inefficient -- just reconstructs each one, then goes thru to fix for lcm.
void rational_reconstruction(std::vector<integer>& num, integer& den, std::vector<integer>& numx, integer& denx, integer& denBound) {
	integer den_tmp, missing_factor;
	den = 1;
	for (size_t i = 0; i < numx.size(); ++i) {
		rational_reconstruction(num[i], den_tmp, numx[i], denx, denBound);
		lcm(missing_factor, den_tmp, den);
		den = missing_factor;
	}
	for (size_t i = 0; i < numx.size(); ++i) {
		rational_reconstruction(num[i], den_tmp, numx[i], denx, denBound);
		integer::divexact (missing_factor, den, den_tmp);
		num[i] *= missing_factor;
	}
} // vector rational_reconstruction
**** */

}// LinBox
#endif // __DYADICTORATIONAL_H

#ifdef TestingDyadicToRational
using namespace LinBox;

#include <vector>
#include <cmath>
#include <linbox/field/PID-integer.h>
#include "linbox/util/timer.h"

int test1(size_t k, size_t dxs, size_t denBs) {
/* Check reconstruction of i/k when it is well approximated
by something over dxs and when the denominator bound (for k) is denBs.
Values of numerator i from -k-2 to k+2 are checked.
*/
	typedef PID_integer Ring; Ring Z;
	typedef Ring::Element Int;

	// k is our denominator
	Int denB = denBs; // denominator bound
	// approximate(i*k^2)/k for i = 0..k-1
	size_t kp = k+2;
	size_t kp2 = 2*kp;
	std::vector<Int> nx(kp2);
	Int dx = dxs;
	for (size_t i = 0; i < kp2 ; ++i) nx[i] = floor(double((i-kp)*dx)/k + 0.5);
	std::vector<Int> n(kp2);
	Int d;
	bool pass = true;
	bool claim;
	int ret = 2;

	// check of individual reconstructions
	Int nn; Z.init(nn);
	int c;
	claim = true;
	// individual reconstructions
	for (size_t i = 0; i < kp2 ; ++i) {
//	std::cout << nx[i] << " " << dx << " " << denB << std::endl;
		c = dyadicToRational(Z, nn, d, nx[i], dx, denB);
//	std::cout << " " << c << " " << nn << " " << d << std::endl;
		claim = c > 0  && (nn*k == (i-kp)*d);
		if ( c == 2 ) claim = claim && d*denB < dx;
		if (c < ret) ret = c;
		if (! claim) ret = 0;
		//if (! claim) std::cout << "F " << pass << claim << ret << std::endl;
		//if (! claim) std::cout << "F2 i " << i << " nx " << nx[i] << " nn " << nn[i] << " d " << d << std::endl;
		pass = pass && claim;
	}
	//std::cout << "i " << pass << claim << ret << std::endl;
	pass = pass && claim;

	// check vector reconstruction
	c = dyadicToRational(Z, n, d, nx, dx, denB);
	claim = 0 < c;
	if (claim) {
		for (size_t i = 0; i < k ; ++i) claim = claim && (n[i] == (long unsigned int) i-kp);
		pass = pass && claim;
		if (!claim) {
			commentator.report() << "first example fails" << std::endl;
			commentator.report() << "data for first example" << std::endl;
			for (size_t i = 0; i < 10 ; ++i)
				commentator.report() << nx[i] << std::endl;
			commentator.report() << dx << " den in" << std::endl;
			commentator.report() << "results for first example" << std::endl;
			for (size_t i = 0; i < 10 ; ++i)
				commentator.report() << n[i] << std::endl;
			commentator.report() << d << " den out" << std::endl;
		}
	}
	if (c < ret) ret = c;
	if (! claim) ret = 0;
	pass = pass && claim;
	//std::cout << "v " << pass << claim << ret << std::endl;

	//return pass;
	return ret;
}

bool testDyadicToRational(size_t k, bool benchmarking = false) {
	typedef PID_integer Ring; Ring Z;
	typedef Ring::Element Int;
	bool pass = true;

	UserTimer clock;
	double totaltime = 0;

	clock.clear(); clock.start();
	pass = pass && 1 == test1(k, k*k, k); // some 1's and some 2's
	pass = pass && 1 == test1(k, k*k, k*k); // all 1's
	pass = pass && 2 == test1(k, k*k+2*k, k+1); // all 2's
	clock.stop(); totaltime += clock.time();

	bool claim = false;

// special case
	Int B = 1000000000;
	Int B2 = B*B;
	Int denB = 4*B+294967296; // 2^32

	Int d;
	Int dx = denB*denB; // 2^64
	size_t k2 = 10;
	std::vector<Int> nx;
	std::vector<Int> n;
	nx.resize(k2);
	n.resize(k2);

	nx[0] =-143*B2-298423624*B-962150784;
	nx[1] = 239*B2+120348615*B+509085366;
	nx[2] =  -4*B2-959983787*B-562075119;
	nx[3] =  27*B2+  8864641*B+551149627;
	nx[4] =  62*B2+971469325*B+838237476;
	nx[5] = 190*B2+559070838*B+297135961;
	nx[6] = 176*B2+172593329*B+811309753;
	nx[7] = -70*B2-861003759*B-845628342;
	nx[8] =-228*B2-416339507*B-338896853;
	nx[9] = -14*B2-398832745*B-762391791;

	claim = 0 < dyadicToRational(Z, n, d, nx, dx, denB);

	pass = pass && claim;
	if (!claim) commentator.report() << "second ratrecon claims failure but should succeed" << std::endl;

	std::vector<Int> ntrue(k2);
	Int dentrue = 691617936;
    ntrue[0] = -5*B-372642434;
	ntrue[1] =  8*B+965263534;
	ntrue[2] =  -185963102;
	ntrue[3] =  1*B+ 12634812;
	ntrue[4] =  2*B+360969365;
	ntrue[5] =  7*B+144570919;
	ntrue[6] =  6*B+605183272;
	ntrue[7] = -2*B-656769182;
	ntrue[8] = -8*B-563941509;
	ntrue[9] =     -539850878;
	claim = (d == dentrue);
	for (size_t i = 0; i < k2 ; ++i) claim = claim && (n[i] == ntrue[i]);
	pass = pass && claim;

	if (!claim)
	{
	commentator.report() << "data for second example" << std::endl;
	for (size_t i = 0; i < k2 ; ++i)
		commentator.report() << nx[i] << std::endl;
	commentator.report() << dx << " den in" << std::endl;
	commentator.report() << "results for second example" << std::endl;
	for (size_t i = 0; i < k2 ; ++i)
		commentator.report() << n[i] << std::endl;
	commentator.report() << d << " den out" << std::endl;
	}

#if 0
// I don't have evidence now that this case should not work
// case where false should be returned.
	denB = 1023*B+948656640;

	dx = 4*B2+611686018*B+427387904;
	size_t k3 = 10;
	nx.resize(k3);
	n.resize(k3);

	nx[0] =  -4*B2-474720817*B-626139615;
	nx[1] =  -9*B2-632772311*B-132715070;
	nx[2] = -19*B2-805041562*B-739831073;
	nx[3] =  35*B2+521355378*B+297487606;
    nx[4] =  27*B2+922294617*B+624925795;
    nx[5] =   1*B2+494454325*B+592253092;
    nx[6] = -27*B2-985233904*B-197462327;
    nx[7] = -20*B2-336729946*B-917106131;
    nx[8] = -42*B2-807924857*B-450940124;
    nx[9] = -27*B2-863559911*B-142533799;


	// this should fail
	claim = 0 < dyadicToRational(Z, n, d, nx, dx, denB);
	//std::cout << "d " << d << " dx " << dx << " denB " << denB << std::endl;

    pass = pass && !claim;
	if (claim) commentator.report() << "third ratrecon falsely claims success" << std::endl;
#endif

// done
	if (benchmarking) commentator.report() << "vec size" << k << ", rat recon time: " << clock << " totaltime " << totaltime << std::endl;
	return pass;
}
#endif // TestingRationalReconstructionSN


/* This file is part of the KDE project
   Copyright (C) 2001-2003 Rob Buis <buis@kde.org>
   Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
   Modified for use within Scribus:
   Copyright (C) 2007 Franz Schmid <Franz.Schmid@altmuehlnet.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KarbonCurveFit.h"
#include <QVector>
#include <math.h>
const qreal Zero = 10e-12;

/*
	An Algorithm for Automatically Fitting Digitized Curves
	by Philip J. Schneider
	from "Graphics Gems", Academic Press, 1990

	http://www.acm.org/pubs/tog/GraphicsGems/gems/FitCurves.c
	http://www.acm.org/pubs/tog/GraphicsGems/gems/README
*/

//#define MAXPOINTS	1000		/* The most points you can have */

class FitVector {
	public:
	FitVector(const QPointF &p)
	{
		m_X=p.x();
		m_Y=p.y();
	}
	
	FitVector(){
		m_X=0;
		m_Y=0;
	}

	FitVector(const QPointF &a, const QPointF &b){
		m_X=a.x()-b.x();
		m_Y=a.y()-b.y();
	}

	void normalize(){
		double len=length();
		if (qFuzzyCompare(len, 0.0)) 
			return;
		m_X/=len; m_Y/=len;
	}

	void negate(){
		m_X = -m_X;
		m_Y = -m_Y;
	}

	void scale(double s){
		double len = length();
		if (qFuzzyCompare(len, 0.0)) 
			return;
		m_X *= s/len;
		m_Y *= s/len;
	}

	double dot(const FitVector &v){
		return ((m_X*v.m_X)+(m_Y*v.m_Y));
	}

	double length(){
		return (double) sqrt(m_X*m_X+m_Y*m_Y); 
	}

	QPointF operator+(const QPointF &p){
		QPointF b(p.x()+m_X,p.y()+m_Y);
		return b;
	}

	public:
		double m_X,m_Y;
};

double distance(const QPointF &p1, const QPointF &p2){
	double dx = (p1.x()-p2.x());
	double dy = (p1.y()-p2.y());
	return sqrt( dx*dx + dy*dy );
}


FitVector ComputeLeftTangent(const QList<QPointF> &points,int end){
	FitVector tHat1( points.at(end+1), points.at(end) );

	tHat1.normalize();

	return tHat1;
}

FitVector ComputeRightTangent(const QList<QPointF> &points,int end){
	FitVector tHat1( points.at(end-1), points.at(end) );

	tHat1.normalize();

	return tHat1;
}

/*
 *  ChordLengthParameterize :
 *	Assign parameter values to digitized points 
 *	using relative distances between points.
 */
static double *ChordLengthParameterize(const QList<QPointF> &points,int first,int last)
{
    int		i;	
    double	*u;			/*  Parameterization		*/

    u = new double[(last-first+1)];

    u[0] = 0.0;
    for (i = first+1; i <= last; i++) {
		u[i-first] = u[i-first-1] +
	  			distance(points.at(i), points.at(i-1));
    }
    double denominator = u[last-first];
    if (qFuzzyCompare(denominator, 0.0))
      denominator = Zero;
    for (i = first + 1; i <= last; i++) {
		u[i-first] = u[i-first] / denominator;
    }

    return(u);
}

static FitVector VectorAdd(FitVector a,FitVector b)
{
    FitVector	c;
    c.m_X = a.m_X + b.m_X;  c.m_Y = a.m_Y + b.m_Y;
    return (c);
}
static FitVector VectorScale(FitVector v,double s)
{
    FitVector result;
    result.m_X = v.m_X * s; result.m_Y = v.m_Y * s;
    return (result);
}

static FitVector VectorSub(FitVector a,FitVector b)
{
    FitVector	c;
    c.m_X = a.m_X - b.m_X; c.m_Y = a.m_Y - b.m_Y;
    return (c);
}

static FitVector ComputeCenterTangent(const QList<QPointF> &points,int center)
{
    FitVector V1, V2, tHatCenter;

    FitVector cpointb( points.at(center-1) );
    FitVector cpoint( points.at(center) );
    FitVector cpointa( points.at(center+1) );

    V1 = VectorSub(cpointb,cpoint);
    V2 = VectorSub(cpoint,cpointa);
    tHatCenter.m_X= ((V1.m_X + V2.m_X)/2.0);
    tHatCenter.m_Y= ((V1.m_Y + V2.m_Y)/2.0);
    tHatCenter.normalize();
    return tHatCenter;
}

/*
 *  B0, B1, B2, B3 :
 *	Bezier multipliers
 */
static double B0(double u)
{
    double tmp = 1.0 - u;
    return (tmp * tmp * tmp);
}


static double B1(double u)
{
    double tmp = 1.0 - u;
    return (3 * u * (tmp * tmp));
}

static double B2(double u)
{
    double tmp = 1.0 - u;
    return (3 * u * u * tmp);
}

static double B3(double u)
{
    return (u * u * u);
}

/*
 *  GenerateBezier :
 *  Use least-squares method to find Bezier control points for region.
 *
 */
QPointF* GenerateBezier(const QList<QPointF> &points, int first, int last, double *uPrime,FitVector tHat1,FitVector tHat2)
{
    int 	i;
//    FitVector	A[MAXPOINTS][2];	/* Precomputed rhs for eqn	*/
    int 	nPts;			/* Number of pts in sub-curve */
    double 	C[2][2];			/* Matrix C		*/
    double 	X[2];			/* Matrix X			*/
    double 	det_C0_C1,		/* Determinants of matrices	*/
    	   	det_C0_X,
	   		det_X_C1;
    double 	alpha_l,		/* Alpha values, left and right	*/
    	   	alpha_r;
    FitVector 	tmp;			/* Utility variable		*/
    QPointF	*curve;
	
    curve = new QPointF[4];
    nPts = last - first + 1;

    QVector< QVector<FitVector> > A(nPts, QVector<FitVector>(2));
    /* Compute the A's	*/
    for (i = 0; i < nPts; i++) {
		FitVector v1, v2;
		v1 = tHat1;
		v2 = tHat2;
		v1.scale(B1(uPrime[i]));
		v2.scale(B2(uPrime[i]));
		A[i][0] = v1;
		A[i][1] = v2;
    }

    /* Create the C and X matrices	*/
    C[0][0] = 0.0;
    C[0][1] = 0.0;
    C[1][0] = 0.0;
    C[1][1] = 0.0;
    X[0]    = 0.0;
    X[1]    = 0.0;

    for (i = 0; i < nPts; i++) {
        C[0][0] += (A[i][0]).dot(A[i][0]);
		C[0][1] += A[i][0].dot(A[i][1]);
		/* C[1][0] += V2Dot(&A[i][0], &A[i][1]);*/	
		C[1][0] = C[0][1];
		C[1][1] += A[i][1].dot(A[i][1]);

		FitVector vfirstp1( points.at(first+i) );
		FitVector vfirst( points.at(first) );
		FitVector vlast( points.at(last) );

		tmp = VectorSub(vfirstp1,
	        VectorAdd(
	          VectorScale(vfirst, B0(uPrime[i])),
		    	VectorAdd(
		      		VectorScale(vfirst, B1(uPrime[i])),
		        			VectorAdd(
	                  		VectorScale(vlast, B2(uPrime[i])),
	                    		VectorScale(vlast, B3(uPrime[i])) ))));
	

        X[0] += A[i][0].dot(tmp);
        X[1] += A[i][1].dot(tmp);
    }

    /* Compute the determinants of C and X	*/
    det_C0_C1 = C[0][0] * C[1][1] - C[1][0] * C[0][1];
    det_C0_X  = C[0][0] * X[1]    - C[0][1] * X[0];
    det_X_C1  = X[0]    * C[1][1] - X[1]    * C[0][1];

    /* Finally, derive alpha values	*/
     if (qFuzzyCompare(det_C0_C1, 0.0)) {
		det_C0_C1 = (C[0][0] * C[1][1]) * 10e-12;
    }
    if (qFuzzyCompare(det_C0_C1, 0.0))
      det_C0_C1 = Zero;
    alpha_l = det_X_C1 / det_C0_C1;
    alpha_r = det_C0_X / det_C0_C1;


    /*  If alpha negative, use the Wu/Barsky heuristic (see text) */
	/* (if alpha is 0, you get coincident control points that lead to
	 * divide by zero in any subsequent NewtonRaphsonRootFind() call. */
    if (alpha_l < 1.0e-6 || alpha_r < 1.0e-6) {
		double	dist = distance(points.at(last),points.at(first)) /
					3.0;

		curve[0] = points.at(first);
		curve[3] = points.at(last);

		tHat1.scale(dist);
		tHat2.scale(dist);

		curve[1] = tHat1 + curve[0];
		curve[2] = tHat2 + curve[3];
        return curve;
    }

    /*  First and last control points of the Bezier curve are */
    /*  positioned exactly at the first and last data points */
    /*  Control points 1 and 2 are positioned an alpha distance out */
    /*  on the tangent vectors, left and right, respectively */
	curve[0] = points.at(first);
	curve[3] = points.at(last);

	tHat1.scale(alpha_l);
	tHat2.scale(alpha_r);

	curve[1] = tHat1 + curve[0];
	curve[2] = tHat2 + curve[3];
    
    return (curve);
}

/*
 *  Bezier :
 *  	Evaluate a Bezier curve at a particular parameter value
 * 
 */
static QPointF BezierII(int degree,QPointF *V, double t)
{
    int 	i, j;		
    QPointF 	Q;	        /* Point on curve at parameter t	*/
    QPointF 	*Vtemp;		/* Local copy of control points		*/

    Vtemp = new QPointF[degree+1];
    
    for (i = 0; i <= degree; i++) {
		Vtemp[i] = V[i];
    }

    /* Triangle computation	*/
    for (i = 1; i <= degree; i++) {	
		for (j = 0; j <= degree-i; j++) {
	    	Vtemp[j].setX((1.0 - t) * Vtemp[j].x() + t * Vtemp[j+1].x());
	    	Vtemp[j].setY((1.0 - t) * Vtemp[j].y() + t * Vtemp[j+1].y());
		}
    }

    Q = Vtemp[0];
    delete[] Vtemp;
    return Q;
}

/*
 *  ComputeMaxError :
 *	Find the maximum squared distance of digitized points
 *	to fitted curve.
*/
static double ComputeMaxError(const QList<QPointF> &points,int first,int last,QPointF *curve,double *u,int *splitPoint)
{
    int		i;
    double	maxDist;		/*  Maximum error		*/
    double	dist;		/*  Current error		*/
    QPointF P;			/*  Point on curve		*/
    FitVector	v;			/*  Vector from point to curve	*/

    *splitPoint = (last - first + 1)/2;
    maxDist = 0.0;
    for (i = first + 1; i < last; i++) {
		P = BezierII(3, curve, u[i-first]);
		v = VectorSub(P, points.at(i));
		dist = v.length();
		if (dist >= maxDist) {
	    	maxDist = dist;
	    	*splitPoint = i;
		}
    }
    return (maxDist);
}


/*
 *  NewtonRaphsonRootFind :
 *	Use Newton-Raphson iteration to find better root.
 */
static double NewtonRaphsonRootFind(QPointF *Q,QPointF P,double u)
{
    double 		numerator, denominator;
    QPointF 		Q1[3], Q2[2];	/*  Q' and Q''			*/
    QPointF		Q_u, Q1_u, Q2_u; /*u evaluated at Q, Q', & Q''	*/
    double 		uPrime;		/*  Improved u			*/
    int 		i;
    
    /* Compute Q(u)	*/
    Q_u = BezierII(3,Q, u);
    
    /* Generate control vertices for Q'	*/
    for (i = 0; i <= 2; i++) {
		Q1[i].setX((Q[i+1].x() - Q[i].x()) * 3.0);
		Q1[i].setY((Q[i+1].y() - Q[i].y()) * 3.0);
    }
    
    /* Generate control vertices for Q'' */
    for (i = 0; i <= 1; i++) {
		Q2[i].setX((Q1[i+1].x() - Q1[i].x()) * 2.0);
		Q2[i].setY((Q1[i+1].y() - Q1[i].y()) * 2.0);
    }
    
    /* Compute Q'(u) and Q''(u)	*/
    Q1_u = BezierII(2, Q1, u);
    Q2_u = BezierII(1, Q2, u);
    
    /* Compute f(u)/f'(u) */
    numerator = (Q_u.x() - P.x()) * (Q1_u.x()) + (Q_u.y() - P.y()) * (Q1_u.y());
    denominator = (Q1_u.x()) * (Q1_u.x()) + (Q1_u.y()) * (Q1_u.y()) +
		      	  (Q_u.x() - P.x()) * (Q2_u.x()) + (Q_u.y() - P.y()) * (Q2_u.y());
    if (qFuzzyCompare(denominator, 0.0)) 
      denominator = Zero;
    /* u = u - f(u)/f'(u) */
    uPrime = u - (numerator/denominator);
    return (uPrime);
}

/*
 *  Reparameterize:
 *	Given set of points and their parameterization, try to find
 *   a better parameterization.
 *
 */
static double *Reparameterize(const QList<QPointF> &points,int first,int last,double *u,QPointF *curve)
{
    int 	nPts = last-first+1;	
    int 	i;
    double	*uPrime;		/*  New parameter values	*/

    uPrime = new double[nPts];
    for (i = first; i <= last; i++) {
		uPrime[i-first] = NewtonRaphsonRootFind(curve, points.at(i), u[i-first]);
    }
    return (uPrime);
}

QPointF *FitCubic(const QList<QPointF> &points,int first,int last,FitVector tHat1,FitVector tHat2,float error,int &width){
	double *u;
	double *uPrime;
	double maxError;
	int splitPoint;
	int nPts;
	double iterationError;
	int maxIterations=4;
	FitVector tHatCenter;
	QPointF *curve;
	int i;

	width=0;

   
	iterationError=error*error;
	nPts = last-first+1;

	if(nPts == 2){
	    	double dist = distance(points.at(last), points.at(first)) / 3.0;

		curve = new QPointF[4];
		
		curve[0] = points.at(first);
		curve[3] = points.at(last);

		tHat1.scale(dist);
		tHat2.scale(dist);
	
		curve[1] = tHat1 + curve[0];
		curve[2] = tHat2 + curve[3];

		width=4;	
		return curve;
	}
    
	/*  Parameterize points, and attempt to fit curve */
	u = ChordLengthParameterize(points, first, last);
	curve = GenerateBezier(points, first, last, u, tHat1, tHat2);


	/*  Find max deviation of points to fitted curve */
	maxError = ComputeMaxError(points, first, last, curve, u, &splitPoint);
	if (maxError < error) {
		delete[] u;
		width=4;	
		return curve;
	}


	/*  If error not too large, try some reparameterization  */
	/*  and iteration */
	if (maxError < iterationError) {
		for (i = 0; i < maxIterations; i++) {
			uPrime = Reparameterize(points, first, last, u, curve);
			delete[] curve;
			curve = GenerateBezier(points, first, last, uPrime, tHat1, tHat2);
			maxError = ComputeMaxError(points, first, last,
					curve, uPrime, &splitPoint);
			if (maxError < error) {
				delete[] u;
				delete[] uPrime;
				width=4;	
				return curve;
			}
			delete[] u;
			u = uPrime;
		}
	}

	/* Fitting failed -- split at max error point and fit recursively */
	delete[] u;
	delete[] curve;
	tHatCenter = ComputeCenterTangent(points, splitPoint);

	int w1,w2;
	QPointF *cu1=nullptr, *cu2=nullptr;
	cu1 = FitCubic(points, first, splitPoint, tHat1, tHatCenter, error,w1);

	tHatCenter.negate();
	cu2 = FitCubic(points, splitPoint, last, tHatCenter, tHat2, error,w2);

	QPointF *newcurve = new QPointF[w1+w2];
	for (int i=0;i<w1;i++)
	{
		newcurve[i]=cu1[i];
	}
	for (int i=0;i<w2;i++)
	{
		newcurve[i+w1]=cu2[i];
	}
	
	delete[] cu1;
	delete[] cu2;
	width=w1+w2;
	return newcurve;
}


QPainterPath bezierFit(const QList<QPointF> &points,float error)
{
	FitVector tHat1, tHat2;

	tHat1 = ComputeLeftTangent(points,0);
	tHat2 = ComputeRightTangent(points,points.count()-1);
	
	int width=0;
	QPointF *curve;
	curve = FitCubic(points,0,points.count()-1,tHat1,tHat2,error,width);
	
	QPainterPath path;

	if(width>3){
		path.moveTo(curve[0]);
		path.cubicTo(curve[1],curve[2],curve[3]);
		for (int i=4;i<width;i+=4){
			path.cubicTo(curve[i+1],curve[i+2],curve[i+3]);	
		}
	}

	delete[] curve;
	return path;
}

QPainterPath bezierFit( const QPolygonF &points, float error )
{
	QList<QPointF> clip = QList<QPointF>::fromVector(points);
	return bezierFit(clip, error);
}

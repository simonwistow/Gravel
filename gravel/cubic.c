
//
// This is taken from my development work.
//
// It illustrates how to approximate 4-point cubic Beziers with
// 3-point quadratic Beziers.  PostScript (and most apps) use
// cubics, Flash uses quadratics.
//
// It uses some MFC list classes, a CPathNode class that supports
// movetos, linetos, curveto2 (quadratic) and curveto3 (cubic).
// A double-precision 2D point class (CDoublePoint) and general
// line class (CDoubleLine)
//
// Regardless, it should give you an idea how to do the approximation.
//
// (Thanks to Paul Haeberli for his help with this)
//


swf_shaperecord ** 
gravel_cubic_to_quadratic(gravel_cubic * p)
{
	int i, j;
	int error = 0;
	SWF_S32 AX, AY, BX, BY, CX, CY, DX, DY;
	SWF_S32 EX, EY;
    double dSplitPoint = 0.5;
	swf_shaperecord *  edge;
	swf_shaperecord ** rec;
	swf_shaperecord *  BC;
	double SideA, SideD;

	if ((rec = (swf_shaperecord **) calloc (1, sizeof (swf_shaperecord *))) == NULL) {
		return NULL;
	}

	AX = p->ax;
	AY = p->ay;

	BX = p->bx;
	BY = p->by;

	CX = p->cx;
	CY = p->cy;

	DX = p->dx;
	DY = p->dy;


    BOOL fForceSplit = FALSE;

    /* Are all the points equal? */
    if (AX == BX && BX == CX && CX == DX && AY == BY && BY == CY && CY == DY) {
        /* its a point! */
        return NULL;
		/* Are the end-points and off-points equal? */
    } else if (AX == BX && CX == DX && AY == BY && CY == DY) {
        /* its a line! */
		edge = gravel_make_line(&error, AX, AY, DX, DY);
		if (error) {
			return NULL;
		}
		*rec = edge;
        return rec;
    } else if (BX != CX || BY != CY) {
        BC = gravel_make_line(&error, BX, BY, CX, CY);

        SideA = gravel_which_side(BC, AX, AY);
        SideD = gravel_which_side(BC, DX, DY);

        if (SideA != SideD && SideA != 0 && SideD != 0)
        {
            // A and D are on opposite sides of BC
            // Cubic must be split in middle
            fForceSplit = TRUE;
        } else if (SideA == 0 && SideD == 0) {
            /* ABCD co-linear: this is a line! */
			edge = gravel_make_line(&error, AX, AY, DX, DY);
			if (error) {
				return NULL;
			}
			*rec = edge;
			return rec;
        }
        else if (SideA == SideD ||                  // same sides
                (SideA == 0 && SideD != 0) ||       // ABC colinear
                (SideA != 0 && SideD == 0))         // BCD colinear
        {
            if (A != D && B == C)                   // BC are equal, E is B
                E = B;
            else if (A == B && B != C && C != D)    // AB are equal, E is  C
                E = C;
            else if (C == D && C != B && B != A)    // CD are equal, E is B
                E = B;
            else
            {
                // A & D are on the same side of BC
                // E is intersection of AB and CD
                CDoubleLine  AB(A, B);
                CDoubleLine  CD(C, D);

                E = AB.Intersection(CD);
            }
        }
    }

    if (!fForceSplit)
    {
        CDoublePoint dpCubicControlPoints[4] = {A, B, C, D};
        CDoublePoint dpQuadraticControlPoints[3] = {A, E, D};

        // Find max. distance between cubic and quadratic
        double      dMaxDistance = 0.0;
        const int   nTestPoints  = 32;
        
        //
        // This just tests the distance between the two curves
        // at an abitrary number of test points.  Really should
        // do something better here, based on the length of the
        // curve probably...
        //

        for (i=1; i<nTestPoints; i++) {
            double  t =(double)i /(double)nTestPoints;

            CDoublePoint dpCubic = EvaluateBezier(3, dpCubicControlPoints, t);
            CDoublePoint dpQuadratic = EvaluateBezier(2, dpQuadraticControlPoints, t);
            CDoublePoint dpDifference = dpCubic - dpQuadratic;

            double  dDistance = dpDifference.SquaredLength();

            if (dDistance > dMaxDistance)
            {
                dSplitPoint = t;
                dMaxDistance = dDistance;
            }
        }

        // Is max distance less than half a pixel
        // 100 = half a twip squared(10*10 = 100)
        if (dMaxDistance < 100.0) {
            // Close enough match - no need to split.
            // Replace Cubic with Quadratic
            m_lNodes.SetAt(p, new CNodeCurveTo2(&dpQuadraticControlPoints[1]));
            delete pCurveTo3;
            return;
        }
    }

    // Split the cubic at dSplitPoint and recurse
    if (SplitCubic(p, dSplitPoint)) {
        // split halves are returned in at p and pNext
        POSITION pNext = p;
        m_lNodes.GetNext(pNext);
        SplitUntilMatches(p);       // recurse left half
        SplitUntilMatches(pNext);   // recurse right half
    }
}


/* ---- */

/* 
 * Split Cubic bezier in two
 */

BOOL gravel_split_cubic(POSITION p, double dSplit)
{
    if (p == m_lNodes.GetHeadPosition()) {
        return FALSE;
	}

    CPathNode*  pCurveTo3 = m_lNodes[p];
    if (pCurveTo3->GetCount() != 3) {
        return FALSE;
	}

    // Copy control points
    CDoublePoint*   pdpPoints = pCurveTo3->GetPoints();
    CDoublePoint    Vtemp[4][4];

    Vtemp[0][0] = GetLastPointOfPrevNode(p);
    for (int j=1; j<4; j++) 
      Vtemp[0][j] = pdpPoints[j-1];


    // Triangle computation
    for (int i=1; i<=3; i++)
    {
        for (j=0 ; j<=3-i; j++)
        {
            Vtemp[i][j].x = (1.0 - dSplit) * Vtemp[i-1][j].x + dSplit * Vtemp[i-1][j+1].x;
            Vtemp[i][j].y = (1.0 - dSplit) * Vtemp[i-1][j].y + dSplit * Vtemp[i-1][j+1].y;          
        }
    }

    // insert left cubic curve
    CDoublePoint dpPoints[3];
    for (j = 1; j <= 3; j++)
        dpPoints[j-1] = Vtemp[j][0];
    m_lNodes.SetAt(p, new CNodeCurveTo3(dpPoints));
    delete pCurveTo3;

    // insert right cubic curve
    for (j = 1; j <= 3; j++)
        dpPoints[j-1] = Vtemp[3-j][j]; 
    m_lNodes.InsertAfter(p, new CNodeCurveTo3(dpPoints));

    return TRUE;
}

/* 
 * Evaluate a Bezier curve at a particular parameter value
 * degree  - The degree of the bezier curve
 * V,      - Array of control points
 * t       - Parametric value to find point for
 */
CDoublePoint gravel_evaluate_bezier(int degree, CDoublePoint* V, double t)
{
  int i, j;

    // Make local of array
    CDoublePoint* Vtemp = new CDoublePoint[degree+1]; 
    for (i=0; i <= degree; i++) 
        Vtemp[i] = V[i];

    // Triangle computation
    for (i = 1; i <= degree; i++) {
        for (int j = 0; j <= degree-i; j++)
        {
            Vtemp[j].x =(1.0 - t) * Vtemp[j].x + t * Vtemp[j+1].x;
            Vtemp[j].y =(1.0 - t) * Vtemp[j].y + t * Vtemp[j+1].y;
        }
    }

    CDoublePoint Q = Vtemp[0];
    delete Vtemp;
    return Q;
}

/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/

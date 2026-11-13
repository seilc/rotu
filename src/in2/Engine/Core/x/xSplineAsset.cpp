#include "xCommon.h"
#include "xSplineAsset.h"

F32 xNurbs::approximate_length(S32 divisions) const
{
    F32 division_size = (end() - start()) / divisions;
    F32 length = 0.0f;

    xVec3 last_point = evaluate(start());

    for (F32 t = division_size + start(); t <= end(); t += division_size) {
        xVec3 point = evaluate(t);
        length += point.Distance(last_point);
        last_point = point;
    }

    return length;
}

S32 xNurbs::find_span(F32 u) const
{
    if (u == knot[controlMaxIndex + 1])
        return controlMaxIndex;

    S32 low = degree;
    S32 high = controlMaxIndex + 1;
    S32 mid = (low + high) / 2;

    while (u < knot[mid] || u >= knot[mid + 1]) {
        if (u < knot[mid]) {
            if (high <= mid)
                break;
            high = mid;
        } else {
            if (low >= mid)
                break;
            low = mid;
        }
        mid = (low + high) / 2;
    }

    return mid;
}

void xNurbs::find_basis_functions(F32 u, S32 i, F32* N) const
{
    N[0] = 1.0f;

    F32 left[5];
    F32 right[5];
    for (S32 j = 1; j <= degree; j++) {
        left[j] = u - knot[i + 1 - j];
        right[j] = knot[i + j] - u;
        F32 saved = 0.0f;
        for (S32 r = 0; r < j; r++) {
            F32 temp = N[r] / (right[r + 1] + left[j - r]);
            N[r] = temp * right[r + 1] + saved;
            saved = temp * left[j - r];
        }
        N[j] = saved;
    }
}

void xNurbs::find_derivative_basis_functions(F32 u, S32 i, S32 order, F32 ders[5][5]) const
{
    // My brain hurts.

    F32 ndu[5][5];
    ndu[0][0] = 1.0f;

    F32 left[5];
    F32 right[5];
    S32 j;
    for (j = 1; j <= degree; j++) {
        left[j] = u - knot[i + 1 - j];
        right[j] = knot[i + j] - u;
        F32 saved = 0.0f;
        for (S32 r = 0; r < j; r++) {
            ndu[j][r] = right[r + 1] + left[j - r];
            F32 temp = ndu[r][j - 1] / ndu[j][r];
            ndu[r][j] = temp * right[r + 1] + saved;
            saved = temp * left[j - r];
        }
        ndu[j][j] = saved;
    }
    for (j = 0; j <= degree; j++) {
        ders[0][j] = ndu[j][degree];
    }
    
    F32 a[2][5];
    S32 r;
    for (r = 0; r <= degree; r++) {
        S32 s1 = 0;
        S32 s2 = 1;
        a[0][0] = 1.0f;
        for (S32 k = 1; k <= order; k++) {
            F32 d = 0.0f;
            S32 rk = r - k;
            S32 pk = degree - k;
            if (r >= k) {
                a[s2][0] = a[s1][0] / ndu[pk + 1][rk];
                d = a[s2][0] * ndu[rk][pk];
            }
            S32 j1, j2;
            if (rk >= -1)
                j1 = 1;
            else
                j1 = -rk;
            if (r - 1 <= pk)
                j2 = k - 1;
            else
                j2 = degree - r;
            for (S32 j = j1; j <= j2; j++) {
                a[s2][j] = (a[s1][j] - a[s1][j - 1]) / ndu[pk + 1][rk + j];
                d += a[s2][j] * ndu[rk + j][pk];
            }
            if (r <= pk) {
                a[s2][k] = -a[s1][k - 1] / ndu[pk + 1][r];
                d += a[s2][k] * ndu[r][pk];
            }
            ders[k][r] = d;

            S32 temp = s1;
            s1 = s2;
            s2 = temp;
        }
    }
    
    r = degree;
    for (S32 k = 1; k <= order; k++) {
        for (S32 j = 0; j <= degree; j++) {
            ders[k][j] *= r;
        }
        r *= degree - k;
    }
}

F32 xNurbs::start() const
{
    return knot[degree];
}

F32 xNurbs::end() const
{
    return knot[controlMaxIndex + 1];
}

xVec3 xNurbs::evaluate(F32 u) const
{
    xVec3 point;
    evaluate(u, point);
    return point;
}

void xNurbs::evaluate(F32 u, xVec3& point) const
{
    S32 span = find_span(u);
    F32 N[5];
    find_basis_functions(u, span, N);

    point = xVec3::m_Null;
    for (S32 i = 0; i <= degree; i++) {
        point.AddScale(control[span - degree + i], N[i]);
    }
}

xVec3 xNurbs::evaluate(F32 u, S32 d) const
{
    xVec3 point;
    evaluate(u, d, point);
    return point;
}

void xNurbs::evaluate(F32 u, S32 d, xVec3& point) const
{
    u = CLAMP(u, start(), end());
    point = xVec3::m_Null;

    if (d > degree) {
        return;
    }
    
    S32 span = find_span(u);
    F32 ders[5][5];
    find_derivative_basis_functions(u, span, d, ders);

    for (S32 j = 0; j <= degree; j++) {
        point.AddScale(control[span - degree + j], ders[d][j]);
    }
}

xVec3 xNurbs::project_point(const xVec3& point, F32* u, F32* distance, F32 guess) const
{
    F32 length2;
    xVec3 curve_point = { 0.0f, 0.0f, 0.0f };

    if (guess > HUGE/2) {
        F32 interval_distance = 0.05f * (end() - start());
        length2 = HUGE;

        for (F32 u = start(); u <= end(); u += interval_distance) {
            xVec3 test_point;
            evaluate(u, test_point);
            F32 test_length2 = point.Distance2(test_point);
            if (test_length2 < length2) {
                length2 = test_length2;
                guess = u;
                curve_point = test_point;
            }
        }
    } else {
        evaluate(guess, curve_point);
        length2 = point.Distance2(curve_point);
    }

    const U32 MAX_DEPTH = 10;
    for (S32 depth = 0; depth < MAX_DEPTH; depth++) {
        if (length2 < 0.01f) {
            if (u)
                *u = guess;
            if (distance) {
                *distance = xsqrt(length2);
            }
            return curve_point;
        }

        xVec3 point_to_curve = {
            curve_point.x - point.x,
            curve_point.y - point.y,
            curve_point.z - point.z
        };

        xVec3 curve_tangent;
        evaluate(guess, 1, curve_tangent);

        // 0.015707318f = sin(0.9deg)
        if (xabs(curve_tangent.dot(point_to_curve)) <
            0.015707318f * xabs(point_to_curve.length() * curve_tangent.length())) {
            if (u)
                *u = guess;
            if (distance) {
                *distance = xsqrt(length2);
            }
            return curve_point;
        }

        xVec3 curve_2nd_derivative;
        evaluate(guess, 2, curve_2nd_derivative);

        F32 last_guess = guess;
        F32 denom = curve_2nd_derivative.dot(point_to_curve) + curve_tangent.length2();
        if (denom < 0.25f) {
            guess += curve_tangent.x * (point.x - curve_point.x) +
                     curve_tangent.y * (point.y - curve_point.y) +
                     curve_tangent.z * (point.z - curve_point.z);
        } else {
            guess -= curve_tangent.dot(point_to_curve) / denom;
        }
        guess = CLAMP(guess, start(), end());

        evaluate(guess, curve_point);

        if (xabs(last_guess - guess) < 0.05f) {
            if (u)
                *u = guess;
            if (distance) {
                *distance = xsqrt(length2);
            }
            return curve_point;
        }
    }

    if (u)
        *u = guess;
    if (distance) {
        *distance = xsqrt(length2);
    }
    return curve_point;
}

bool xNurbs::advance_u(F32 start_u, F32 distance, bool forward, F32& new_u) const
{
    F32 spline_start = start();
    F32 spline_end = end();

    if (start_u < spline_start) start_u = spline_start; else if (start_u > spline_end) start_u = spline_end;

    new_u = start_u;
    
    xVec3 velocity_vec;
    evaluate(start_u, 1, velocity_vec);

    F32 vel_mag = xVec3NormalizeSafe(velocity_vec);
    if (vel_mag == 0.0f) {
        return false;
    }

    if (forward == true) {
        new_u = start_u + distance / vel_mag;
        if (new_u > spline_end) {
            new_u = spline_end;
            return false;
        }
    } else {
        new_u = start_u - distance / vel_mag;
        if (new_u < spline_start) {
            new_u = spline_start;
            return false;
        }
    }

    return true;
}

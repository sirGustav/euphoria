#ifndef CORE_NUMERIC_H
#define CORE_NUMERIC_H

namespace euphoria::core
{
    bool
    IsEqual(float lhs, float rhs);

    bool
    IsZero(float r);

    float
    ZeroOrValue(float r);

    float
    Floor(float v);

    float
    Ceil(float v);

    int
    Floori(float v);

    int
    Ceili(float v);


    /** Calculates the sign as a positive or a negative int.
    @returns 1 if r is greater than 0, -1 if not.
    @see Abs()
    */
    int
    Sign(float r);


    int
    Sign(int r);


    /** Returns either 1 or -1 */
    float
    Sign(bool b);


    float
    Lerp(float f, float scale, float t);

    /** Performs a single interpolating step to transform a old value to a new value.
    Larger smoothing values result in a smoother but less resposive path.

    Example:
    @code mousex = Curve( ( MousePos.X - OldMousePos.X ) * Sensitivity(), mousex, 6
    ); @endcode

    @param new_value is the target value
    @param old_value is the current value
    @param smoothing_value is the smoothing value > 0, the lower, the less smooth

    @returns The smoothed result.
    */
    float
    Curve(float new_value, float old_value, float smoothing_value);

    float
    Square(float r);

    float
    Sqrt(float r);

    float
    Log(float r);

    float
    Abs(float r);

    float
    Min(float lhs, float rhs);

    int
    Min(int lhs, int rhs);

    float
    Max(float lhs, float rhs);

    int
    Max(int lhs, int rhs);

    float
    Mod(float numer, float denumer);

    bool
    IsWithinInclusivei(int min, int c, int max);

    /** Rounds a value to the nearest nice value.
        If the granularity is 1 the function rounds to the closest integer, at .5 the
        closest half integer, at 2 even integers etc...
        @param num the number to round
        @param gran the granularity
        */
    float
    Round(float num, float gran);

    constexpr float
    Pi()
    {
        return 3.1415926535897932384626433832795f;
    }

    constexpr float
    HalfPi()
    {
        return Pi() / 2.0f;
    }

    constexpr bool
    TrueValue()
    {
        return true;
    }

    constexpr bool
    FalseValue()
    {
        return false;
    }

    template <typename T>
    T
    DefaultIfCloseToZero(T value, T def, T epsilon)
    {
        if(Abs(value) < epsilon)
            return def;
        else
            return value;
    }

    /// returns true for a index each Nth loop. pass zero to always return false
    /// useful for example adding a newline each 10th or 15th line
    template <typename I>
    constexpr bool
    Each(I i, I each)
    {
        return each && (i % each) == each - 1;
    }

}  // namespace euphoria::core

#endif  // CORE_NUMERIC_H
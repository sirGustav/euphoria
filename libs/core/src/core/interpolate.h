#pragma once

// todo(Gustav): this seems way to complicated... why is there a deque here?
// I think we could just remove it and use a 'is tweening or not' logic

#include "core/assert.h"
#include <deque>

namespace euphoria::core
{
    namespace easing
    {
#define FUN(NAME, FUNC) float NAME(float value);
        // Linear interpolation (no easing)
        FUN(Linear, LinearInterpolation)

        // Quadratic easing; p^2
        FUN(QuadIn, QuadraticEaseIn)
        FUN(QuadOut, QuadraticEaseOut)
        FUN(Quad, QuadraticEaseInOut)

        // Cubic easing; p^3
        FUN(CubicIn, CubicEaseIn)
        FUN(CubicOut, CubicEaseOut)
        FUN(Cubic, CubicEaseInOut)

        // Quartic easing; p^4
        FUN(QuartIn, QuarticEaseIn)
        FUN(QuartOut, QuarticEaseOut)
        FUN(Quart, QuarticEaseInOut)

        // Quintic easing; p^5
        FUN(QuintIn, QuinticEaseIn)
        FUN(QuintOut, QuinticEaseOut)
        FUN(Quint, QuinticEaseInOut)

        // Sine wave easing; sin(p * PI/2)
        FUN(SineIn, SineEaseIn)
        FUN(SineOut, SineEaseOut)
        FUN(Sine, SineEaseInOut)

        // Circular easing; sqrt(1 - p^2)
        FUN(CircIn, CircularEaseIn)
        FUN(CircOut, CircularEaseOut)
        FUN(Circ, CircularEaseInOut)

        // Exponential easing, base 2
        FUN(ExpIn, ExponentialEaseIn)
        FUN(ExpOut, ExponentialEaseOut)
        FUN(Exp, ExponentialEaseInOut)

        // Exponentially-damped sine wave easing
        FUN(ElasticIn, ElasticEaseIn)
        FUN(ElasticOut, ElasticEaseOut)
        FUN(Elastic, ElasticEaseInOut)

        // Overshooting cubic easing;
        FUN(BackIn, BackEaseIn)
        FUN(BackOut, BackEaseOut)
        FUN(Back, BackEaseInOut)

        // Exponentially-decaying bounce easing
        FUN(BounceIn, BounceEaseIn)
        FUN(BounceOut, BounceEaseOut)
        FUN(Bounce, BounceEaseInOut)
#undef FUN
    }

    enum class InterpolationType
    {
#define FUN(NAME, FUNC) NAME,
        // Linear interpolation (no easing)
        FUN(Linear, LinearInterpolation)

        // Quadratic easing; p^2
        FUN(QuadIn, QuadraticEaseIn) FUN(QuadOut, QuadraticEaseOut)
                FUN(Quad, QuadraticEaseInOut)

        // Cubic easing; p^3
        FUN(CubicIn, CubicEaseIn) FUN(CubicOut, CubicEaseOut)
                FUN(Cubic, CubicEaseInOut)

        // Quartic easing; p^4
        FUN(QuartIn, QuarticEaseIn) FUN(QuartOut, QuarticEaseOut)
                FUN(Quart, QuarticEaseInOut)

        // Quintic easing; p^5
        FUN(QuintIn, QuinticEaseIn) FUN(QuintOut, QuinticEaseOut)
                FUN(Quint, QuinticEaseInOut)

        // Sine wave easing; sin(p * PI/2)
        FUN(SineIn, SineEaseIn) FUN(SineOut, SineEaseOut)
                FUN(Sine, SineEaseInOut)

        // Circular easing; sqrt(1 - p^2)
        FUN(CircIn, CircularEaseIn) FUN(CircOut, CircularEaseOut)
                FUN(Circ, CircularEaseInOut)

        // Exponential easing, base 2
        FUN(ExpIn, ExponentialEaseIn) FUN(ExpOut, ExponentialEaseOut)
                FUN(Exp, ExponentialEaseInOut)

        // Exponentially-damped sine wave easing
        FUN(ElasticIn, ElasticEaseIn) FUN(ElasticOut, ElasticEaseOut)
                FUN(Elastic, ElasticEaseInOut)

        // Overshooting cubic easing;
        FUN(BackIn, BackEaseIn) FUN(BackOut, BackEaseOut)
                FUN(Back, BackEaseInOut)

        // Exponentially-decaying bounce easing
        FUN(BounceIn, BounceEaseIn) FUN(BounceOut, BounceEaseOut)
                FUN(Bounce, BounceEaseInOut)
#undef FUN
                        INVALID
    };

    struct FloatTransform
    {
        static float
        Transform(float from, float v, float to);
    };

    // Transform should have a function Transform(T from, float zeroToOne, T to)
    template <typename Type, typename Transform>
    struct Interpolate
    {
        using This = Interpolate<Type, Transform>;

        explicit Interpolate(Type v)
            : value_(v), from_(v), position_in_current_interpolation_(0.0f)
        {}

        [[nodiscard]] bool
        HasSteps() const
        {
            return !data_.empty();
        }

        const Type&
        Debug_GetFrom() const
        {
            return from_;
        }

        const Type&
        GetValue() const
        {
            return value_;
        }

        This&
        SetValue(const Type& t)
        {
            value_ = t;
            Clear();
            return *this;
        }

        operator const Type&() const
        {
            return GetValue();
        }

        Interpolate&
        operator=(const Type& rhs)
        {
            SetValue(rhs);
            return *this;
        }

        bool
        UpdateValueFromInterpolationPosition()
        {
            if(data_.empty())
            {
                return false;
            }
            const InterpolationData<Type>& d = data_.front();
            if(d.type != nullptr)
            {
                const float interpolated = d.type(position_in_current_interpolation_);
                value_ = Transform::Transform(from_, interpolated, d.target);
            }
            return true;
        }

        void
        Update(float adt)
        {
            float dt = adt;

            while(dt > 0.0f)
            {
                if(data_.empty())
                {
                    UpdateValueFromInterpolationPosition();
                    return;
                }
                const InterpolationData<Type>& d = data_.front();
                position_in_current_interpolation_ += dt / d.time;
                const bool over = position_in_current_interpolation_ >= 1.0f;

                // update the delta time for the next interpolation step
                if(over)
                {
                    dt = (1 - position_in_current_interpolation_) * d.time;
                }
                else
                {
                    dt = -1;
                }

                if(over)
                {
                    position_in_current_interpolation_ -= 1.0f;
                    value_ = d.target;
                    data_.pop_front();
                    from_ = value_;
                }
            }

            UpdateValueFromInterpolationPosition();
        }

        This&
        Clear()
        {
            data_.clear();
            position_in_current_interpolation_ = 0.0f;
            return *this;
        }

        This&
        Sleep(float time)
        {
            AddInterpolation(nullptr, value_, time);
            return *this;
        }

#define FUN(NAME, FUNC) \
    This& NAME(const Type& target, float time) \
    { \
        AddInterpolation(easing::NAME, target, time); \
        return *this; \
    }
        // Linear interpolation (no easing)
        FUN(Linear, LinearInterpolation)

        // Quadratic easing; p^2
        FUN(QuadIn, QuadraticEaseIn)
        FUN(QuadOut, QuadraticEaseOut)
        FUN(Quad, QuadraticEaseInOut)

        // Cubic easing; p^3
        FUN(CubicIn, CubicEaseIn)
        FUN(CubicOut, CubicEaseOut)
        FUN(Cubic, CubicEaseInOut)

        // Quartic easing; p^4
        FUN(QuartIn, QuarticEaseIn)
        FUN(QuartOut, QuarticEaseOut)
        FUN(Quart, QuarticEaseInOut)

        // Quintic easing; p^5
        FUN(QuintIn, QuinticEaseIn)
        FUN(QuintOut, QuinticEaseOut)
        FUN(Quint, QuinticEaseInOut)

        // Sine wave easing; sin(p * PI/2)
        FUN(SineIn, SineEaseIn)
        FUN(SineOut, SineEaseOut)
        FUN(Sine, SineEaseInOut)

        // Circular easing; sqrt(1 - p^2)
        FUN(CircIn, CircularEaseIn)
        FUN(CircOut, CircularEaseOut)
        FUN(Circ, CircularEaseInOut)

        // Exponential easing, base 2
        FUN(ExpIn, ExponentialEaseIn)
        FUN(ExpOut, ExponentialEaseOut)
        FUN(Exp, ExponentialEaseInOut)

        // Exponentially-damped sine wave easing
        FUN(ElasticIn, ElasticEaseIn)
        FUN(ElasticOut, ElasticEaseOut)
        FUN(Elastic, ElasticEaseInOut)

        // Overshooting cubic easing;
        FUN(BackIn, BackEaseIn)
        FUN(BackOut, BackEaseOut)
        FUN(Back, BackEaseInOut)

        // Exponentially-decaying bounce easing
        FUN(BounceIn, BounceEaseIn)
        FUN(BounceOut, BounceEaseOut)
        FUN(Bounce, BounceEaseInOut)
#undef FUN

        This&
        Add(InterpolationType type, const Type& target, float time)
        {
#define FUN(NAME, FUNC) \
    case InterpolationType::NAME: \
        AddInterpolation(easing::NAME, target, time); \
        return *this;
            switch(type)
            {
                // Linear interpolation (no easing)
                FUN(Linear, LinearInterpolation)

                // Quadratic easing; p^2
                FUN(QuadIn, QuadraticEaseIn)
                FUN(QuadOut, QuadraticEaseOut)
                FUN(Quad, QuadraticEaseInOut)

                // Cubic easing; p^3
                FUN(CubicIn, CubicEaseIn)
                FUN(CubicOut, CubicEaseOut)
                FUN(Cubic, CubicEaseInOut)

                // Quartic easing; p^4
                FUN(QuartIn, QuarticEaseIn)
                FUN(QuartOut, QuarticEaseOut)
                FUN(Quart, QuarticEaseInOut)

                // Quintic easing; p^5
                FUN(QuintIn, QuinticEaseIn)
                FUN(QuintOut, QuinticEaseOut)
                FUN(Quint, QuinticEaseInOut)

                // Sine wave easing; sin(p * PI/2)
                FUN(SineIn, SineEaseIn)
                FUN(SineOut, SineEaseOut)
                FUN(Sine, SineEaseInOut)

                // Circular easing; sqrt(1 - p^2)
                FUN(CircIn, CircularEaseIn)
                FUN(CircOut, CircularEaseOut)
                FUN(Circ, CircularEaseInOut)

                // Exponential easing, base 2
                FUN(ExpIn, ExponentialEaseIn)
                FUN(ExpOut, ExponentialEaseOut)
                FUN(Exp, ExponentialEaseInOut)

                // Exponentially-damped sine wave easing
                FUN(ElasticIn, ElasticEaseIn)
                FUN(ElasticOut, ElasticEaseOut)
                FUN(Elastic, ElasticEaseInOut)

                // Overshooting cubic easing;
                FUN(BackIn, BackEaseIn)
                FUN(BackOut, BackEaseOut)
                FUN(Back, BackEaseInOut)

                // Exponentially-decaying bounce easing
                FUN(BounceIn, BounceEaseIn)
                FUN(BounceOut, BounceEaseOut)
                FUN(Bounce, BounceEaseInOut)
#undef FUN

            case InterpolationType::INVALID:
            default: DIE("Unhandled case");
            }
            // ignore invalid interpolation type
            return *this;
        }


    private:
        using EasingFunction = float (*)(float);

        template <typename TType>
        struct InterpolationData
        {
            InterpolationData(const TType& t) : type(nullptr), target(t), time(0.0f) {}

            EasingFunction type; // how to interpolate
            TType target; // target value
            float time; // time to transition
        };
        Type value_; // current value

        Type from_; // starting value
        float position_in_current_interpolation_; // goes from 0 to 1
        std::deque<InterpolationData<Type>> data_;

        void
        AddInterpolation(EasingFunction type, const Type& target, float time)
        {
            ASSERTX(time >= 0.0f, time);

            if(data_.empty())
            {
                from_ = value_;
            }

            InterpolationData<Type> d(target);
            d.type = type;
            d.time = time;
            data_.push_back(d);
        }
    };

    using FloatInterpolate = Interpolate<float, FloatTransform>;
}

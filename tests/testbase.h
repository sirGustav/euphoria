#ifndef TESTS_TESTBASE_H
#define TESTS_TESTBASE_H

#include "tests/approx.h"

#include "core/vec3.h"
#include "core/axisangle.h"
#include "core/quat.h"


template <>
bool
approximately_equal(vec3f const& lhs, vec3f const& rhs, const ApproxData& data);

template <>
bool
approximately_equal(vec3f::Unit const& lhs, vec3f::Unit const& rhs,
                    const ApproxData& data);

template <>
bool
approximately_equal(quatf const& lhs, quatf const& rhs, const ApproxData& data);

template <>
bool
approximately_equal(AxisAngle const& lhs, AxisAngle const& rhs,
                    const ApproxData& data);


#endif  // TESTS_TESTBASE_H

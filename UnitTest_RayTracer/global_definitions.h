#pragma once
#ifndef __YS_GLOBAL_DEFINITIONS_HPP__
#define __YS_GLOBAL_DEFINITIONS_HPP__


#define YS_DECIMAL_IS_DOUBLE


#include "maths/maths.h"

#ifdef YS_DECIMAL_IS_DOUBLE
#define ASSERT_DECIMAL_EQ ASSERT_DOUBLE_EQ
#define EXPECT_DECIMAL_EQ EXPECT_DOUBLE_EQ
#else
#define ASSERT_DECIMAL_EQ ASSERT_FLOAT_EQ
#define EXPECT_DECIMAL_EQ EXPECT_FLOAT_EQ
#ifdef _DEBUG
#define YS_REDECIMAL_HAS_PRECISE 1
#endif
#endif


#endif // __YS_GLOBAL_DEFINTIONS_HPP__

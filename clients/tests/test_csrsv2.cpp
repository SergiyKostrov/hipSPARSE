/* ************************************************************************
 * Copyright 2018 Advanced Micro Devices, Inc.
 * ************************************************************************ */

#include "testing_csrsv2.hpp"
#include "utility.hpp"

#include <hipsparse.h>
#include <gtest/gtest.h>
#include <unistd.h>
#include <vector>
#include <string>

typedef hipsparseIndexBase_t base;
typedef hipsparseOperation_t op;
typedef hipsparseDiagType_t diag;
typedef hipsparseFillMode_t fill;

typedef std::tuple<int, double, base, op, diag, fill> csrsv2_tuple;
typedef std::tuple<double, base, op, diag, fill, std::string> csrsv2_bin_tuple;

int csrsv2_M_range[] = {-1, 0, 50, 647};

double csrsv2_alpha_range[] = {1.0, 2.3, -3.7};

base csrsv2_idxbase_range[] = {HIPSPARSE_INDEX_BASE_ZERO, HIPSPARSE_INDEX_BASE_ONE};
op csrsv2_op_range[]        = {HIPSPARSE_OPERATION_NON_TRANSPOSE};
diag csrsv2_diag_range[]    = {HIPSPARSE_DIAG_TYPE_NON_UNIT};
fill csrsv2_fill_range[]    = {HIPSPARSE_FILL_MODE_LOWER, HIPSPARSE_FILL_MODE_UPPER};

std::string csrsv2_bin[] = {"rma10.bin",
                            "mac_econ_fwd500.bin",
                            "mc2depi.bin",
                            "scircuit.bin",
                            "ASIC_320k.bin",
                            "bmwcra_1.bin",
                            "nos1.bin",
                            "nos2.bin",
                            "nos3.bin",
                            "nos4.bin",
                            "nos5.bin",
                            "nos6.bin"};

class parameterized_csrsv2 : public testing::TestWithParam<csrsv2_tuple>
{
    protected:
    parameterized_csrsv2() {}
    virtual ~parameterized_csrsv2() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};

class parameterized_csrsv2_bin : public testing::TestWithParam<csrsv2_bin_tuple>
{
    protected:
    parameterized_csrsv2_bin() {}
    virtual ~parameterized_csrsv2_bin() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};

Arguments setup_csrsv2_arguments(csrsv2_tuple tup)
{
    Arguments arg;
    arg.M         = std::get<0>(tup);
    arg.alpha     = std::get<1>(tup);
    arg.idx_base  = std::get<2>(tup);
    arg.transA    = std::get<3>(tup);
    arg.diag_type = std::get<4>(tup);
    arg.fill_mode = std::get<5>(tup);
    arg.timing    = 0;
    return arg;
}

Arguments setup_csrsv2_arguments(csrsv2_bin_tuple tup)
{
    Arguments arg;
    arg.M         = -99;
    arg.alpha     = std::get<0>(tup);
    arg.idx_base  = std::get<1>(tup);
    arg.transA    = std::get<2>(tup);
    arg.diag_type = std::get<3>(tup);
    arg.fill_mode = std::get<4>(tup);
    arg.timing    = 0;

    // Determine absolute path of test matrix
    std::string bin_file = std::get<5>(tup);

    // Get current executables absolute path
    char path_exe[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", path_exe, sizeof(path_exe) - 1);
    if(len < 14)
    {
        path_exe[0] = '\0';
    }
    else
    {
        path_exe[len - 14] = '\0';
    }

    // Matrices are stored at the same path in matrices directory
    arg.filename = std::string(path_exe) + "matrices/" + bin_file;

    return arg;
}

TEST(csrsv2_bad_arg, csrsv2_float) { testing_csrsv2_bad_arg<float>(); }

TEST_P(parameterized_csrsv2, csrsv2_float)
{
    Arguments arg = setup_csrsv2_arguments(GetParam());

    hipsparseStatus_t status = testing_csrsv2<float>(arg);
    EXPECT_EQ(status, HIPSPARSE_STATUS_SUCCESS);
}

TEST_P(parameterized_csrsv2, csrsv2_double)
{
    Arguments arg = setup_csrsv2_arguments(GetParam());

    hipsparseStatus_t status = testing_csrsv2<double>(arg);
    EXPECT_EQ(status, HIPSPARSE_STATUS_SUCCESS);
}

TEST_P(parameterized_csrsv2_bin, csrsv2_bin_float)
{
    Arguments arg = setup_csrsv2_arguments(GetParam());

    hipsparseStatus_t status = testing_csrsv2<float>(arg);
    EXPECT_EQ(status, HIPSPARSE_STATUS_SUCCESS);
}

TEST_P(parameterized_csrsv2_bin, csrsv2_bin_double)
{
    Arguments arg = setup_csrsv2_arguments(GetParam());

    hipsparseStatus_t status = testing_csrsv2<double>(arg);
    EXPECT_EQ(status, HIPSPARSE_STATUS_SUCCESS);
}

INSTANTIATE_TEST_CASE_P(csrsv2,
                        parameterized_csrsv2,
                        testing::Combine(testing::ValuesIn(csrsv2_M_range),
                                         testing::ValuesIn(csrsv2_alpha_range),
                                         testing::ValuesIn(csrsv2_idxbase_range),
                                         testing::ValuesIn(csrsv2_op_range),
                                         testing::ValuesIn(csrsv2_diag_range),
                                         testing::ValuesIn(csrsv2_fill_range)));

INSTANTIATE_TEST_CASE_P(csrsv2_bin,
                        parameterized_csrsv2_bin,
                        testing::Combine(testing::ValuesIn(csrsv2_alpha_range),
                                         testing::ValuesIn(csrsv2_idxbase_range),
                                         testing::ValuesIn(csrsv2_op_range),
                                         testing::ValuesIn(csrsv2_diag_range),
                                         testing::ValuesIn(csrsv2_fill_range),
                                         testing::ValuesIn(csrsv2_bin)));

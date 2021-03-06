/*
 * Copyright(c) 2017 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "gtest/gtest.h"

#include <cstdint>
#include <fstream>
#include <istream>
#include <iterator>

#include "framework/framework.hpp"

class VmeHmeSystemTests : public testing::Test {
protected:
  virtual void TearDown() { std::remove(output_file_.c_str()); }

  bool VerifyTest() {
    const char *argv[] = {"compute_samples",
                          "vme_hme",
                          const_cast<char *>(input_file_.c_str()),
                          const_cast<char *>(output_file_.c_str()),
                          "--width",
                          "176",
                          "--height",
                          "144",
                          "--qp",
                          "45",
                          "-f",
                          "50",
                          nullptr};
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    compute_samples::Framework framework;
    testing::internal::CaptureStdout();
    framework.run(argc, argv);
    testing::internal::GetCapturedStdout();

    std::ifstream out(output_file_, std::ios::binary);
    std::string reference_file = "hme_";
    reference_file += input_file_;
    std::ifstream ref(reference_file, std::ios::binary);
    EXPECT_TRUE(out.good() && ref.good());
    EXPECT_TRUE(!out.eof() && !ref.eof());

    std::istreambuf_iterator<char> out_iter(out);
    std::istreambuf_iterator<char> ref_iter(ref);
    std::istreambuf_iterator<char> eos_iter;
    while (ref_iter != eos_iter && out_iter != eos_iter) {
      if (*ref_iter++ != *out_iter++)
        return false;
    }

    return out_iter == eos_iter && ref_iter == eos_iter;
  }

  const std::string input_file_ = "foreman_176x144.yuv";
  const std::string output_file_ = "output_foreman_176x144.yuv";
};

TEST_F(VmeHmeSystemTests, ReturnsReferenceImage) {
  ASSERT_EQ(VerifyTest(), true);
}

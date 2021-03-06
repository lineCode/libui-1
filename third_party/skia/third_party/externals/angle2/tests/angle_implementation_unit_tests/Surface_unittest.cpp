//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "libANGLE/Surface.h"
#include "libANGLE/renderer/SurfaceImpl.h"

namespace
{

class MockSurfaceImpl : public rx::SurfaceImpl
{
  public:
    virtual ~MockSurfaceImpl() { destroy(); }

    MOCK_METHOD0(initialize, egl::Error());
    MOCK_METHOD0(swap, egl::Error());
    MOCK_METHOD4(postSubBuffer, egl::Error(EGLint, EGLint, EGLint, EGLint));
    MOCK_METHOD2(querySurfacePointerANGLE, egl::Error(EGLint, void**));
    MOCK_METHOD1(bindTexImage, egl::Error(EGLint));
    MOCK_METHOD1(releaseTexImage, egl::Error(EGLint));
    MOCK_METHOD1(setSwapInterval, void(EGLint));
    MOCK_CONST_METHOD0(getWidth, EGLint());
    MOCK_CONST_METHOD0(getHeight, EGLint());
    MOCK_CONST_METHOD0(getWindowHandle, EGLNativeWindowType());

    MOCK_METHOD0(destroy, void());
};

class SurfaceTest : public testing::Test
{
  protected:
    virtual void SetUp()
    {
        mImpl = new MockSurfaceImpl;
        EXPECT_CALL(*mImpl, destroy());
        mSurface = new egl::Surface(mImpl);
    }

    virtual void TearDown()
    {
        delete mSurface;
    }

    MockSurfaceImpl *mImpl;
    egl::Surface *mSurface;
};

TEST_F(SurfaceTest, DestructionDeletesImpl)
{
    MockSurfaceImpl *impl = new MockSurfaceImpl;
    EXPECT_CALL(*impl, destroy()).Times(1).RetiresOnSaturation();

    egl::Surface *surface = new egl::Surface(impl);
    delete surface;

    // Only needed because the mock is leaked if bugs are present,
    // which logs an error, but does not cause the test to fail.
    // Ordinarily mocks are verified when destroyed.
    testing::Mock::VerifyAndClear(impl);
}

} // namespace

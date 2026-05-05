#define REAPERAPI_IMPLEMENT
#include <config.h>
#include <gtest/gtest.h>
#include <reaper_plugin_functions.h>

namespace PROJECT_NAME
{

void GetVersion(int* majorOut, int* minorOut, int* patchOut, int* tweakOut, char* commitOut,
                int commitOut_sz, char* branchOut, int branchOut_sz);

TEST(MyPluginTest, GetVersionReturnsCmakeVersion)
{
    int major{-1};
    int minor{-1};
    int patch{-1};
    int tweak{-1};
    char commit[64]{};
    char branch[64]{};
    GetVersion(&major, &minor, &patch, &tweak, commit, sizeof(commit), branch, sizeof(branch));

    EXPECT_EQ(major, PROJECT_VERSION_MAJOR);
    EXPECT_EQ(minor, PROJECT_VERSION_MINOR);
    EXPECT_EQ(patch, PROJECT_VERSION_PATCH);
    EXPECT_EQ(tweak, PROJECT_VERSION_TWEAK);
}

} // namespace PROJECT_NAME

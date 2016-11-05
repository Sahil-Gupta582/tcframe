#include "gmock/gmock.h"

#include "BaseEteTests.cpp"

using ::testing::Eq;
using ::testing::Test;
using ::testing::UnorderedElementsAre;

namespace tcframe {

class GenerationEteTests : public BaseEteTests {};

TEST_F(GenerationEteTests, Normal) {
    ASSERT_THAT(execStatus("cd ete/normal && ../scripts/generate.sh normal"), Eq(0));

    EXPECT_THAT(ls("ete/normal/tc"), UnorderedElementsAre(
           "normal_sample_1.in",
           "normal_sample_1.out",
           "normal_1.in",
           "normal_1.out",
           "normal_2.in",
           "normal_2.out"
    ));
}

TEST_F(GenerationEteTests, Subtasks) {
    ASSERT_THAT(execStatus("cd ete/subtasks && ../scripts/generate.sh subtasks"), Eq(0));

    EXPECT_THAT(ls("ete/subtasks/tc"), UnorderedElementsAre(
            "subtasks_sample_1.in",
            "subtasks_sample_1.out",
            "subtasks_sample_2.in",
            "subtasks_sample_2.out",
            "subtasks_1_1.in",
            "subtasks_1_1.out",
            "subtasks_1_2.in",
            "subtasks_1_2.out",
            "subtasks_2_1.in",
            "subtasks_2_1.out"
    ));
}

TEST_F(GenerationEteTests, Multi) {
    ASSERT_THAT(execStatus("cd ete/multi && ../scripts/generate.sh multi"), Eq(0));

    EXPECT_THAT(ls("ete/multi/tc"), UnorderedElementsAre(
            "multi_sample.in",
            "multi_sample.out",
            "multi_1.in",
            "multi_1.out",
            "multi_2.in",
            "multi_2.out"
    ));
}

}
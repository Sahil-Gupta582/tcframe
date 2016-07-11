#pragma once

#include <functional>
#include <vector>
#include <set>
#include <sstream>
#include <string>

#include "GeneratorLogger.hpp"
#include "GenerationResult.hpp"
#include "TestGroupGenerationResult.hpp"
#include "TestGroupGenerator.hpp"
#include "tcframe/core.hpp"
#include "tcframe/io.hpp"
#include "tcframe/os.hpp"
#include "tcframe/testcase.hpp"

using std::function;
using std::istringstream;
using std::vector;
using std::set;
using std::string;

namespace tcframe {

class TestSuiteGenerator {
private:
    TestGroupGenerator* testGroupGenerator_;
    IOManipulator* ioManipulator_;
    OperatingSystem* os_;
    GeneratorLogger* logger_;

public:
    virtual ~TestSuiteGenerator() {}

    TestSuiteGenerator(
            TestGroupGenerator* testGroupGenerator,
            IOManipulator* ioManipulator,
            OperatingSystem* os,
            GeneratorLogger* logger)
            : testGroupGenerator_(testGroupGenerator)
            , ioManipulator_(ioManipulator)
            , os_(os)
            , logger_(logger)
    {}

    virtual GenerationResult generate(const TestSuite& testSuite, const GeneratorConfig& config) {
        logger_->logIntroduction();
        GenerationResult result = doGenerate(testSuite, config);
        logger_->logResult(result);
        return result;
    }

private:
    GenerationResult doGenerate(const TestSuite& testSuite, const GeneratorConfig& config) {
        os_->forceMakeDir(config.testCasesDir());

        vector<TestGroupGenerationResult> testGroupResults;
        testGroupResults.push_back(generateSampleTests(testSuite, config));
        for (TestGroupGenerationResult result : generateOfficialTests(testSuite, config)) {
            testGroupResults.push_back(result);
        }
        return GenerationResult(testGroupResults);
    }

    TestGroupGenerationResult generateSampleTests(const TestSuite& testSuite, const GeneratorConfig& config) {
        vector<SampleTestCase> sampleTests = testSuite.sampleTests();
        vector<TestCase> testCases;
        for (int testCaseNo = 1; testCaseNo <= sampleTests.size(); testCaseNo++) {
            SampleTestCase sampleTestCase = sampleTests[testCaseNo - 1];
            TestCase testCase = TestCaseBuilder()
                    .setId(TestCaseIdCreator::create(config.slug(), 0, testCaseNo))
                    .setSubtaskIds(sampleTestCase.subtaskIds())
                    .setApplier([=] {
                        istream* in = new istringstream(sampleTestCase.content());
                        ioManipulator_->parseInput(in);
                    })
                    .build();
            testCases.push_back(testCase);
        }

        return testGroupGenerator_->generate(TestGroup(0, testCases), config);
    }

    vector<TestGroupGenerationResult> generateOfficialTests(const TestSuite& testSuite, const GeneratorConfig& config) {
        vector<TestGroupGenerationResult> results;
        for (const OfficialTestGroup& officialTestGroup : testSuite.officialTests()) {
            results.push_back(generateOfficialTestGroup(officialTestGroup, testSuite.inputFinalizer(), config));
        }
        return results;
    }

    TestGroupGenerationResult generateOfficialTestGroup(
            const OfficialTestGroup& officialTestGroup,
            const function<void()>& inputFinalizer,
            const GeneratorConfig& config) {

        vector<OfficialTestCase> officialTestCases = officialTestGroup.officialTestCases();
        vector<TestCase> testCases;
        for (int testCaseNo = 1; testCaseNo <= officialTestCases.size(); testCaseNo++) {
            OfficialTestCase officialTestCase = officialTestCases[testCaseNo - 1];
            string testCaseId = TestCaseIdCreator::create(
                    config.slug(),
                    officialTestGroup.id(),
                    testCaseNo);
            TestCase testCase = TestCaseBuilder()
                    .setId(testCaseId)
                    .setDescription(officialTestCase.description())
                    .setSubtaskIds(officialTestGroup.subtaskIds())
                    .setApplier([=] {officialTestCase.closure()(); inputFinalizer();})
                    .build();
            testCases.push_back(testCase);
        }

        return testGroupGenerator_->generate(TestGroup(officialTestGroup.id(), testCases), config);
    }
};

class TestSuiteGeneratorFactory {
public:
    virtual ~TestSuiteGeneratorFactory() {}

    virtual TestSuiteGenerator* create(
            TestGroupGenerator* testGroupGenerator,
            IOManipulator* ioManipulator,
            OperatingSystem* os,
            GeneratorLogger* logger) {

        return new TestSuiteGenerator(testGroupGenerator, ioManipulator, os, logger);
    }
};

}

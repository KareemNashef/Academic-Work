package OOP.Solution;

import OOP.Provided.OOPResult;
import java.util.Map;

public class OOPTestSummary {

    // Private variables
    int numSuccesses = 0;
    int numFailures = 0;
    int numErrors = 0;
    int numExceptionMismatches = 0;

    // Constructor
    public OOPTestSummary(Map<String, OOPResult> testMap) {

        // Iterate over the given map results
        testMap.forEach((key, value) -> {

            // Increment the corresponding counter
            switch (value.getResultType()) {
                case SUCCESS:
                    numSuccesses++;
                    break;
                case FAILURE:
                    numFailures++;
                    break;
                case ERROR:
                    numErrors++;
                    break;
                case EXPECTED_EXCEPTION_MISMATCH:
                    numExceptionMismatches++;
                    break;
            }
        });

    }

    // Getters
    public int getNumSuccesses() {
        return numSuccesses;
    }
    public int getNumFailures() {
        return numFailures;
    }
    public int getNumExceptionMismatches() {
        return numExceptionMismatches;
    }
    public int getNumErrors() {
        return numErrors;
    }

}

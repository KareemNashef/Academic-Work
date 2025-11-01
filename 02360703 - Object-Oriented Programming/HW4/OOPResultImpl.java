package OOP.Solution;

import OOP.Provided.OOPResult;

public class OOPResultImpl implements OOPResult {

    // Private variables
    private OOPTestResult resultType;
    private String message;

    // Constructor
    public OOPResultImpl(OOPTestResult inResultType, String inMessage) {
        resultType = inResultType;
        message = inMessage;
    }

    // Returns the result type
    @Override
    public OOPTestResult getResultType() {
        return resultType;
    }

    // Returns the message in case of an error
    @Override
    public String getMessage() {
        return message;
    }

    // Equals contract between two test results
    @Override
    public boolean equals(Object obj) {

        // Make sure the input is valid
        if (!(obj instanceof OOPResultImpl)) {
            return false;
        }

        // Check if the two results are the same
        if (resultType != ((OOPResultImpl) obj).getResultType())
            return false;

        // Check if the two messages are null
        if (message == null) {
            return ((OOPResultImpl) obj).message == null;
        }
        if (((OOPResultImpl) obj).message == null) {
            return false;
        }

        // Check if the two messages are the same
        return message.equals(((OOPResultImpl) obj).message);
    }

}

package OOP.Solution;

import OOP.Provided.OOPExpectedException;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

public class OOPExpectedExceptionImpl implements OOPExpectedException {

    // Private variables
    private Class<? extends Exception> expectedException;
    private List<String> expectedMessages;

    // Constructor
    public OOPExpectedExceptionImpl() {
        this.expectedException = null;
        this.expectedMessages = new ArrayList<>();
    }

    // Return the expected exception
    @Override
    public Class<? extends Exception> getExpectedException() {
        return expectedException;
    }

    // Set the expected exception
    @Override
    public OOPExpectedException expect(Class<? extends Exception> expected) {
        this.expectedException = expected;
        return this;
    }

    // Set the expected message
    @Override
    public OOPExpectedException expectMessage(String msg) {
        this.expectedMessages.add(msg);
        return this;
    }

    // Check the thrown exception
    @Override
    public boolean assertExpected(Exception e) {

        // Check if null
        if (expectedException == null) return e == null;
        if (e == null || !expectedException.isAssignableFrom(e.getClass())) return false;


        String inMessage = e.getMessage();
        if (inMessage == null) {
            return expectedMessages.isEmpty();
        }

        return expectedMessages.stream()
                .filter(currentMessage -> !(inMessage.contains(currentMessage)))
                .collect(Collectors.toSet())
                .isEmpty();
    }

    public static OOPExpectedException none() {
        return new OOPExpectedExceptionImpl();
    }
}

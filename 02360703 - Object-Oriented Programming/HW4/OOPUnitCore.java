package OOP.Solution;

import OOP.Provided.OOPAssertionFailure;
import OOP.Provided.OOPExceptionMismatchError;
import OOP.Provided.OOPExpectedException;
import OOP.Provided.OOPResult;
import java.util.stream.Collectors;
import java.lang.annotation.Annotation;
import java.lang.reflect.*;
import java.util.*;

public class OOPUnitCore {


    public static void assertEquals(Object expected, Object actual) throws OOPAssertionFailure {
        // Check if both are the same/null
        if (expected == actual) {
            return;
        }

        // Check if only one is null
        if (expected == null || actual == null) {
            throw new OOPAssertionFailure();
        }

        // Check if they're equal
        if (!expected.equals(actual)) {
            throw new OOPAssertionFailure();
        }
    }

    public static void fail() throws OOPAssertionFailure {
        throw new OOPAssertionFailure();
    }

    public static OOPTestSummary runClass(Class<?> testClass) throws IllegalArgumentException {
        // Set an empty string and send it to the other runClass
        return runClass(testClass, "");
    }

    public static OOPTestSummary runClass(Class<?> testClass, String tag) throws IllegalArgumentException {
        // Check if the given parameter is null
        if (testClass == null || tag == null) {
            throw new IllegalArgumentException();
        }

        // Check if the given class is a test class
        if (!testClass.isAnnotationPresent(OOPTestClass.class)) {
            throw new IllegalArgumentException();
        }

        // Initializing the summary map
        Map<String, OOPResult> summaryMap = new HashMap<>();

        try {

            // Initialize a new instance of testClass
            Constructor<?> constructor = testClass.getDeclaredConstructor();
            constructor.setAccessible(true);
            Object instance = constructor.newInstance();

            // Run OOPSetup methods
            runMethods(instance, null, OOPSetup.class);


            // Acquiring the list of methods
            List<Method> methods = getAllMethods(instance);

            // Filter and sort methods
            methods = methods.stream()
                    .filter(method -> method.isAnnotationPresent(OOPTest.class) &&
                            (tag.isEmpty() || method.getAnnotation(OOPTest.class).tag().equals(tag)))
                    .sorted(Comparator.comparingInt(method -> method.getAnnotation(OOPTest.class).order()))
                    .toList();

            for (Method method : methods) {

                // Get expected exception
                OOPExpectedException expectedException = null;

                for (Field field : testClass.getDeclaredFields()) {

                    // Check if the field is annotated with OOPExceptionRule
                    if (field.isAnnotationPresent(OOPExceptionRule.class)) {

                        // Make the field accessible, even if it's private
                        field.setAccessible(true);
                        try {
                            expectedException = (OOPExpectedException) field.get(instance);
                        } catch (Exception ignored) {}
                    }
                }

                try {

                    // Run OOPBefore methods
                    if(runMethods(instance, method.getName(), OOPBefore.class)) {

                        // Add error to summary and move on to the next test
                        summaryMap.put(method.getName(), new OOPResultImpl(OOPResult.OOPTestResult.ERROR, null));
                        continue;
                    }

                    // Run the test method
                    method.setAccessible(true);
                    method.invoke(instance);

                    // Update the expected exception
                    for (Field field : testClass.getDeclaredFields()) {

                        // Check if the field is annotated with OOPExceptionRule
                        if (field.isAnnotationPresent(OOPExceptionRule.class)) {

                            // Make the field accessible, even if it's private
                            field.setAccessible(true);
                            try {
                                expectedException = (OOPExpectedException) field.get(instance);
                            } catch (Exception ignored) {}
                        }
                    }

                    // Check if we expected an exception
                    if (expectedException == null || expectedException.getExpectedException() == null) // No exception expected - add success to the summary
                        // Add success to the summary
                        summaryMap.put(method.getName(), new OOPResultImpl(OOPResult.OOPTestResult.SUCCESS, null));
                    else
                        // Add error to the summary
                        summaryMap.put(method.getName(), new OOPResultImpl(OOPResult.OOPTestResult.ERROR, expectedException.getClass().getName()));
                }

                // Caught an exception
                catch (InvocationTargetException caughtException) {

                    // Update the expected exception
                    for (Field field : testClass.getDeclaredFields()) {

                        // Check if the field is annotated with OOPExceptionRule
                        if (field.isAnnotationPresent(OOPExceptionRule.class)) {

                            // Make the field accessible, even if it's private
                            field.setAccessible(true);
                            try {
                                expectedException = (OOPExpectedException) field.get(instance);
                            } catch (Exception ignored) {}
                        }
                    }

                    // Check if the exception is an assertion failure
                    if (caughtException.getCause() instanceof OOPAssertionFailure)  // Add failure to the summary
                        summaryMap.put(method.getName(), new OOPResultImpl(OOPResult.OOPTestResult.FAILURE, caughtException.getCause().getMessage()));
                    else {

                        // Check if we expected an exception
                        if (expectedException == null || expectedException.getExpectedException() == null) // No exception expected - add error to the summary
                            summaryMap.put(method.getName(), new OOPResultImpl(OOPResult.OOPTestResult.ERROR, caughtException.getClass().getName()));
                        else {

                            // Check if the expected exception was thrown
                            if (expectedException.assertExpected((Exception) caughtException.getTargetException()))  // Add success to the summary
                                summaryMap.put(method.getName(), new OOPResultImpl(OOPResult.OOPTestResult.SUCCESS, null));

                            // Unexpected exception was thrown
                            else // add mismatch to the summary
                                summaryMap.put(method.getName(), new OOPResultImpl(OOPResult.OOPTestResult.EXPECTED_EXCEPTION_MISMATCH, (new OOPExceptionMismatchError(expectedException.getExpectedException(), caughtException.getClass())).getMessage()));
                        }
                    }
                }

                // Run OOPAfter methods
                if (runMethods(instance, method.getName(), OOPAfter.class))
                    // Adjust to error in the summary
                    summaryMap.put(method.getName(), new OOPResultImpl(OOPResult.OOPTestResult.ERROR, null));
            }
        }

        catch (Exception ignored) { boolean ignore = true; }

        return new OOPTestSummary(summaryMap);
    }

    // Helper method to run instance methods
    private static boolean runMethods(Object instance, String methodName, Class<? extends Annotation> annotationClass) {

        // Get a list of all methods
        List<Method> methodsToExecute = getAllMethods(instance);

        // Determine iteration order based on the given annotation
        if (annotationClass.equals(OOPSetup.class) || annotationClass.equals(OOPBefore.class)) {

            // For OOPSetup and OOPBefore, iterate from parent to child, for OOPAfter, leave unchanged
            Collections.reverse(methodsToExecute);
        }

        // Variable to check if a before/after method caught an exception
        boolean caughtException = false;

        // Filtering the list of methods
        methodsToExecute = methodsToExecute.stream()
                .filter(method -> method.isAnnotationPresent(annotationClass))
                .collect(Collectors.toList());

        // Iterate over the methods
        for (Method method : methodsToExecute) {

            // Check what annotation it is
            if (annotationClass.equals(OOPSetup.class))
                runSetupMethod(instance, method);
            else if (annotationClass.equals(OOPBefore.class) || annotationClass.equals(OOPAfter.class))
                if (runABMethod(instance, method, methodName, annotationClass))
                    caughtException = true;
        }

        return caughtException;
    }

    // Run a setup method
    private static void runSetupMethod(Object instance, Method method) {
        method.setAccessible(true);
        try {
            method.invoke(instance);
        } catch (Exception ignored) {}
    }

    // Run a Before/After method
    private static boolean runABMethod(Object instance, Method method, String methodName, Class<? extends Annotation> annotationClass) {

        // Check the methods defined by the annotation
        String[] value = (annotationClass.equals(OOPBefore.class) ?
                method.getAnnotation(OOPBefore.class).value() :
                method.getAnnotation(OOPAfter.class).value());

        // Check if the test method is in the list
        if (Arrays.asList(value).contains(methodName)) {

            // Backup the fields
            List<Object> backup = backupFields(instance);

            // Run the method
            method.setAccessible(true);
            try {
                method.invoke(instance);
            } catch (Exception ignored) {
                // Restore the fields
                restoreFields(instance, backup);
                return true;
            }
        }

        return false;
    }

    // Backup the fields of a given instance
    private static List<Object> backupFields(Object instance) {

        List<Object> backup = new ArrayList<>();
        Field[] fields = instance.getClass().getDeclaredFields();

        for (Field field : fields) {
            try {
                field.setAccessible(true);
                Object fieldValue = field.get(instance);
                backup.add(cloneFieldValue(fieldValue));
            } catch (Exception ignored) {}
        }

        return backup;
    }

    // Restore the fields of a given instance
    private static void restoreFields(Object instance, List<Object> backup) {

        Field[] fields = instance.getClass().getDeclaredFields();
        for (int i = 0; i < backup.size(); i++) {
            try {
                fields[i].setAccessible(true);
                fields[i].set(instance, backup.get(i));
            } catch (Exception ignored) {}
        }
    }

    // Helper method to clone an object
    private static Object cloneFieldValue(Object fieldValue) throws Exception {

        // Check if the field is cloneable
        if (fieldValue instanceof Cloneable) {
            return fieldValue.getClass().getMethod("clone").invoke(fieldValue);
        }

        // Check if the field has a copy constructor
        Constructor<?> copyConstructor = findCopyConstructor(fieldValue.getClass());
        if (copyConstructor != null) {
            copyConstructor.setAccessible(true);
            return copyConstructor.newInstance(fieldValue);
        }

        // Otherwise, just return the field value
        return fieldValue;
    }

    // Helper method to find a copy constructor
    private static Constructor<?> findCopyConstructor(Class<?> inClass) {
        for (Constructor<?> constructor : inClass.getDeclaredConstructors()) {
            if (constructor.getParameterCount() == 1 &&
                    constructor.getParameterTypes()[0].equals(inClass)) {
                return constructor;
            }
        }
        return null;
    }

    // Helper method to acquire all methods in a given class
    private static List<Method> getAllMethods(Object instance) {

        // Build the class hierarchy
        List<Class<?>> classHierarchy = new ArrayList<>();
        Class<?> currentClass = instance.getClass();
        while(currentClass != null) {

            classHierarchy.add(currentClass);
            currentClass = currentClass.getSuperclass();
        }

        // Get a list of all methods
        List<Method> methodsToExecute = new ArrayList<>();
        for (Class<?> current : classHierarchy) {

            // Iterate over current class methods
            for (Method method : current.getDeclaredMethods()) {

                // Check if there's already a method with the same name and same parameters
                boolean isOverridden = false;
                for (Method existingMethod : methodsToExecute) {
                    if (method.getName().equals(existingMethod.getName()) && Arrays.equals(method.getParameterTypes(), existingMethod.getParameterTypes())) {
                        isOverridden = true;
                        break;
                    }
                }

                // Add the method
                if (!isOverridden) methodsToExecute.add(method);
            }
        }

        // Return the list
        return methodsToExecute;
    }
}

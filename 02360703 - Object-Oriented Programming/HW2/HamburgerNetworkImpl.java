package OOP.Solution;

import OOP.Provided.HamburgerNetwork;
import OOP.Provided.HungryStudent;
import OOP.Provided.Restaurant;

import java.util.*;
import java.util.stream.Collectors;

public class HamburgerNetworkImpl implements HamburgerNetwork {

    // Network variables
    Set<Restaurant> restaurants = new HashSet<>();
    Set<HungryStudent> hungryStudents = new HashSet<>();

    public HamburgerNetworkImpl() {}

    @Override
    public HungryStudent joinNetwork(int id, String name) throws HungryStudent.StudentAlreadyInSystemException {
        // Initializing a new student
        HungryStudent newStudent = new HungryStudentImpl(id, name);

        // Checking if a student with the same ID already exists in the network
        for (HungryStudent iterStudent : hungryStudents) {if (iterStudent.equals(newStudent)) throw new HungryStudent.StudentAlreadyInSystemException();}

        // Adding the student to the network
        hungryStudents.add(newStudent);

        return newStudent;
    }

    @Override
    public Restaurant addRestaurant(int id, String name, int dist, Set<String> menu) throws Restaurant.RestaurantAlreadyInSystemException {
        // Initializing a new restaurant
        Restaurant newRes = new RestaurantImpl(id, name, dist, menu);

        // Checking if a restaurant with the same ID already exists in the network
        for (Restaurant iterRest : restaurants) {if (iterRest.equals(newRes)) throw new Restaurant.RestaurantAlreadyInSystemException();}

        // Adding the student to the network
        restaurants.add(newRes);

        return newRes;
    }

    @Override
    public Collection<HungryStudent> registeredStudents() {
        return hungryStudents;
    }

    @Override
    public Collection<Restaurant> registeredRestaurants() {
        return restaurants;
    }

    @Override
    public HungryStudent getStudent(int id) throws HungryStudent.StudentNotInSystemException {
        // Initializing a temp student for "equal" checking
        HungryStudent tempStudent = new HungryStudentImpl(id,"temp");

        // Iterating over the student list
        for (HungryStudent student : hungryStudents) {
            // Checking the IDs
            if(student.equals(tempStudent)) return student;
        }

        // Student not found
        throw new HungryStudent.StudentNotInSystemException();
    }

    @Override
    public Restaurant getRestaurant(int id) throws Restaurant.RestaurantNotInSystemException {
        // Initializing a temp restaurant for "equal" checking
        Restaurant tempRes = new RestaurantImpl(id,"temp",0,new HashSet<>());

        // Iterating over the restaurant list
        for (Restaurant restaurant : restaurants) {
            // Checking the IDs
            if(restaurant.equals(tempRes)) return restaurant;
        }

        // Student not found
        throw new Restaurant.RestaurantNotInSystemException();
    }

    @Override
    public HamburgerNetwork addConnection(HungryStudent inStudent1, HungryStudent inStudent2) throws HungryStudent.StudentNotInSystemException, HungryStudent.ConnectionAlreadyExistsException, HungryStudent.SameStudentException {
        // Checking if the students exist in the network
        if (!(hungryStudents.contains(inStudent1))) throw new HungryStudent.StudentNotInSystemException();
        if (!(hungryStudents.contains(inStudent2))) throw new HungryStudent.StudentNotInSystemException();

        // Adding a connection from one student to another
        inStudent1.addFriend(inStudent2);
        inStudent2.addFriend(inStudent1);

        return this;
    }

    @Override
    public Collection<Restaurant> favoritesByRating(HungryStudent inStudent) throws HungryStudent.StudentNotInSystemException {
        // Checking if the student exists in the network
        if (!(hungryStudents.contains(inStudent))) throw new HungryStudent.StudentNotInSystemException();

        // Sorting the friends by their IDs
        List<HungryStudent> sortedFriends = new ArrayList<>(inStudent.getFriends());
        sortedFriends.sort(HungryStudent::compareTo);

        // Creating a new set to hold the restaurants
        Set<Restaurant> sortedRes = new LinkedHashSet<>();

        // Iterating over the friends
        for (HungryStudent iterStudent : sortedFriends) {
            // Sorting and adding their favorites to the set using a stream
            sortedRes.addAll(iterStudent.favoritesByRating(0).stream()
                    .sorted(Comparator.comparing(Restaurant::averageRating).reversed()
                            .thenComparing(Restaurant::distance)
                            .thenComparing(Restaurant::compareTo))
                    .toList());
        }

        return new ArrayList<>(sortedRes);
    }

    @Override
    public Collection<Restaurant> favoritesByDist(HungryStudent inStudent) throws HungryStudent.StudentNotInSystemException {
        // Checking if the student exists in the network
        if (!(hungryStudents.contains(inStudent))) throw new HungryStudent.StudentNotInSystemException();

        // Sorting the friends by their IDs
        List<HungryStudent> sortedFriends = new ArrayList<>(inStudent.getFriends());
        sortedFriends.sort(HungryStudent::compareTo);

        // Creating a new set to hold the restaurants
        Set<Restaurant> sortedRes = new LinkedHashSet<>();

        // Iterating over the friends
        for (HungryStudent iterStudent : sortedFriends) {
            // Sorting and adding their favorites to the set using a stream
            sortedRes.addAll(iterStudent.favoritesByRating(0).stream()
                    .sorted(Comparator.comparing(Restaurant::distance).reversed()
                            .thenComparingDouble(Restaurant::averageRating).reversed()
                            .thenComparing(Restaurant::compareTo))
                    .toList());
        }

        return new ArrayList<>(sortedRes);
    }

    @Override
    public String toString() {
        String netData = "";

        // Following the format
        netData = netData.concat("Registered students: ");

        // Getting a sorted list of students
        List<HungryStudent> sortedStudents = new ArrayList<>(this.hungryStudents);
        if (!sortedStudents.isEmpty()) sortedStudents.sort(Comparator.comparing(entry -> ((HungryStudentImpl) entry).studentID));

        // Iterating over the students
        for (int i = 0; i < sortedStudents.size(); i++) {
            netData = netData.concat(String.valueOf(((HungryStudentImpl)sortedStudents.get(i)).studentID));
            if (i != sortedStudents.size() - 1) netData = netData.concat(", ");
        }
        netData = netData.concat(".\n");

        netData = netData.concat("Registered restaurants: ");

        // Getting a sorted list of restaurants
        List<Restaurant> sortedRestaurants = new ArrayList<>(this.restaurants);
        if (!sortedRestaurants.isEmpty()) sortedRestaurants.sort(Comparator.comparing(entry -> ((RestaurantImpl) entry).resID));

        // Iterating over the restaurants
        for (int i = 0; i < sortedRestaurants.size(); i++) {
            netData = netData.concat(String.valueOf(((RestaurantImpl)sortedRestaurants.get(i)).resID));
            if (i != sortedRestaurants.size() - 1) netData = netData.concat(", ");
        }
        netData = netData.concat(".\n");



        netData = netData.concat("Students:\n");
        for (HungryStudent student : sortedStudents) {
            netData = netData.concat(((HungryStudentImpl)student).studentID + " -> [");

            // Sorting the friends by their IDs
            List<HungryStudent> sortedFriends = new ArrayList<>(student.getFriends());
            if (!sortedFriends.isEmpty()) sortedFriends.sort(HungryStudent::compareTo);

            for(int i = 0; i < sortedFriends.size(); i++) {
                netData = netData.concat(String.valueOf(((HungryStudentImpl)sortedFriends.get(i)).studentID));
                if(i != sortedFriends.size() - 1) netData = netData.concat(", ");
            }
            netData = netData.concat("].\n");
        }

        return netData.concat("End students.");
    }

    @Override
    public boolean getRecommendation(HungryStudent inStudent, Restaurant inRes, int inPred) throws HungryStudent.StudentNotInSystemException, Restaurant.RestaurantNotInSystemException, ImpossibleConnectionException {
        // Checking if the student exists in the network
        if (!(hungryStudents.contains(inStudent))) throw new HungryStudent.StudentNotInSystemException();

        // Checking if the restaurant exists in the network
        if (!(restaurants.contains(inRes))) throw new Restaurant.RestaurantNotInSystemException();

        // Checking if the condition is valid
        if (inPred < 0) throw new ImpossibleConnectionException();

        // Making a set of required students
        Set<HungryStudent> deepFriends = new HashSet<>();
        deepFriends.add(inStudent);

        // Going deep
        for (int i = 0; i < inPred; i++) {
            Set<HungryStudent> loopTemp = new HashSet<>();
            for (HungryStudent friend : deepFriends) {loopTemp.addAll(friend.getFriends());}
            deepFriends.addAll(loopTemp);
        }

        // Making a set of all recommended restaurants
        Set<Restaurant> deepRestaurants = new HashSet<>();
        for (HungryStudent friend : deepFriends) {deepRestaurants.addAll(friend.favoritesByRating(0));}

        // Checking if the given restaurant is in the set
        return deepRestaurants.contains(inRes);
    }
}

package OOP.Solution;

import OOP.Provided.HungryStudent;
import OOP.Provided.Restaurant;
import OOP.Solution.RestaurantImpl;
import java.util.*;
import java.util.stream.Collectors;

public class HungryStudentImpl implements HungryStudent{

    // Student variables
    int studentID;
    String studentName;
    Restaurant studentFavorite;
    Set<Restaurant> studentFavorites = new HashSet<>();
    Set<HungryStudent> friendsList = new HashSet<>();

    public HungryStudentImpl(int id, String name) {
        // We assume that the input is valid, we don't check it
        this.studentID = id;
        this.studentName = name;
    }

    @Override
    public HungryStudent favorite(Restaurant inRes) throws UnratedFavoriteRestaurantException {
        // Checking if the student has rated the restaurant
        if (!((RestaurantImpl) inRes).ratedBy(this)) throw new UnratedFavoriteRestaurantException();

        // Set the restaurant as the student's favorite and adding it to the set
        this.studentFavorite = inRes;
        this.studentFavorites.add(inRes);

        return this;
    }

    @Override
    public Collection<Restaurant> favorites() {
        return studentFavorites;
    }

    @Override
    public HungryStudent addFriend(HungryStudent inStudent) throws SameStudentException, ConnectionAlreadyExistsException {
        // Check if the input is the same as this
        if (this.equals(inStudent)) throw new SameStudentException();

        // Check if they're already friends
        if (this.friendsList.contains(inStudent)) throw new ConnectionAlreadyExistsException();

        // Adding the friend to the list
        friendsList.add(inStudent);

        return this;
    }

    @Override
    public Set<HungryStudent> getFriends() {
        return friendsList;
    }

    @Override
    public Collection<Restaurant> favoritesByRating(int minRating) {

        // Using stream to get the list
        return studentFavorites.stream()
                .filter(restaurant -> restaurant.averageRating() >= minRating)                  // Filter the list
                .sorted(Comparator.comparingDouble(Restaurant::averageRating).reversed()        // Sort by average rating
                        .thenComparing(Restaurant::distance)                                    // If equal, sort by distance
                        .thenComparing(Restaurant::compareTo))                                  // If equal, sort by ID
                .collect(Collectors.toList());                                                  // Return sorted list

    }

    @Override
    public Collection<Restaurant> favoritesByDist(int maxDistance) {

        // Using stream to get the list
        return studentFavorites.stream()
                .filter(restaurant -> restaurant.distance() <= maxDistance)                     // Filter the list
                .sorted(Comparator.comparing(Restaurant::distance).reversed()                   // Sort by distance
                        .thenComparingDouble(Restaurant::averageRating).reversed()              // If equal, sort by average rating
                        .thenComparing(Restaurant::compareTo))                                  // If equal, sort by ID
                .collect(Collectors.toList());                                                  // Return sorted list

    }

    @Override
    public boolean equals(Object inStudent) {
        // Checking if the object is the correct type
        if (!(inStudent instanceof HungryStudent)) return false;

        // Otherwise checking the IDs
        return this.studentID == ((HungryStudentImpl) inStudent).studentID;
    }

    @Override
    public int hashCode() {
        return Objects.hash(this.studentID);
    }

    @Override
    public String toString() {
        String studentData = "";

        // Following the format
        studentData = studentData.concat("Hungry student: " + this.studentName + ".\n");
        studentData = studentData.concat("Id: " + this.studentID + ".\n");
        studentData = studentData.concat("Favorites: ");

        // Convert the set to a list
        List<Restaurant> sortedFavorites = new ArrayList<>(this.studentFavorites);

        // Sort the list
        if (!sortedFavorites.isEmpty()) sortedFavorites.sort(Comparator.comparing(entry -> ((RestaurantImpl) entry).resName));

        // Iterating over the favorites
        for (int i = 0; i < sortedFavorites.size(); i++) {
            studentData = studentData.concat(((RestaurantImpl)sortedFavorites.get(i)).resName);
            if (i != sortedFavorites.size() - 1) studentData = studentData.concat(", ");
        }

        return studentData.concat(".");
    }

    @Override
    public int compareTo(HungryStudent inStudent) {
        // Comparing the IDs
        return Integer.compare(this.studentID, ((HungryStudentImpl) inStudent).studentID);
    }
}
